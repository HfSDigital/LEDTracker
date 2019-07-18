#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	blob::ID_Counter = 0;
	
	// ---------------------------------
	// GET IP-ADDRESS OF WIFI-ADAPTER
	// ---------------------------------
	siteLocalInterfaces = ofxNet::NetworkUtils::listNetworkInterfaces(ofxNet::NetworkUtils::SITE_LOCAL);
	string myIp = "0.0.0.0";
	bool wifiAdapterConnected = false;

	for (const auto& interface : siteLocalInterfaces)
	{
		cout << "Interface: [" << interface.name() << "] (" << interface.address().toString() << ")" << std::endl;
		if (interface.name().compare("WLAN") == 0) {
			wifiAdapterConnected = true;
			myIp = interface.address().toString();
		}
	}
	if (!wifiAdapterConnected) cout << "No WiFi Adapter Connected!" << endl;

	// ---------------------------------
	// CAMERA
	// ---------------------------------
	vidGrabber.setVerbose(true);
	vidGrabber.setDeviceID(2);
	vidGrabber.initGrabber(1280, 720);
	vidGrabber.videoSettings();

	// ---------------------------------
	// TRACKER
	// ---------------------------------
	colorImg.allocate(1280, 720);
	grayImage.allocate(1280, 720);
	grayBg.allocate(1280, 720);
	grayDiff.allocate(1280, 720);

	// ---------------------------------
	// GUI
	// ---------------------------------
	search.addListener(this, &ofApp::scanForShoes);
	identify.addListener(this, &ofApp::identifyShoes);

	guiParameter.setName("Settings");
	guiParameter.add(ipAddress.set("IP: ", myIp));

	guiTracking.setName("Tracking");
	guiTracking.add(threshold.set("threshold", 30, 0, 100));
	guiTracking.add(threshold_blobRange.set("blob Range", 10, 0, 100));
	int _maxArea = vidGrabber.getWidth() * vidGrabber.getHeight() * 0.01;
	guiTracking.add(minArea.set("minArea", 5, 1, _maxArea/10));
	guiTracking.add(nConsidered.set("nConsidered", 10, 1, _maxArea/10));
	guiTracking.add(maxArea.set("maxArea", 200, _maxArea / 100, _maxArea));
	guiParameter.add(guiTracking);

	setupShoeGUI();

	gui.loadFromFile("settings.xml");
	ipAddress = myIp;				// re-set ip adress after settings.xml is loaded

	// ---------------------------------
	// OSC 
	// ---------------------------------
	oscReceiver.setup(oscReceiverPort);
}

//--------------------------------------------------------------
void ofApp::update(){
	vidGrabber.update();

	if (state == IDENTIFY) {
		if (identify_currentShoe < shoes.size() && identify_findNextShoe) {
			shoes.at(identify_currentShoe)->LEDStatus = true;
			identify_findNextShoe = false;
			//identify_currentShoe++;
		}
		else if (identify_currentShoe >= shoes.size()) {
			state = PLAY;
		}
	}
	//do we have a new frame?
	if (vidGrabber.isFrameNew()) {
		colorImg.setFromPixels(vidGrabber.getPixels());
		grayImage = colorImg; // convert our color image to a grayscale image

		grayImage.threshold(threshold);
		
		contourFinder.findContours(grayImage, minArea, maxArea, nConsidered, false, true);
		
		for (int j = 0; j < contourFinder.blobs.size(); j++) {
			bool blobExists = false;
			for (int i = 0; i < blobs.size(); i++) {
				float dist = ofDist(contourFinder.blobs.at(j).centroid.x, contourFinder.blobs.at(j).centroid.y, blobs.at(i)->position.x, blobs.at(i)->position.y);
				if (dist < threshold_blobRange) {
					blobExists = true;
					blobs.at(i)->updatePos(contourFinder.blobs.at(j).centroid.x, contourFinder.blobs.at(j).centroid.y);
				}
			}
			if (!blobExists)
			{
				float x = contourFinder.blobs.at(j).centroid.x;
				float y = contourFinder.blobs.at(j).centroid.y;
				blobs.push_back(make_shared<blob>(x, y));
				
				if (state == IDENTIFY) 
				{
					trackedShoes[shoes.at(identify_currentShoe)] = blobs.back();
					blobs.back()->connectedShoe = shoes.at(identify_currentShoe);
					shoes.at(identify_currentShoe)->LEDStatus = false;
					identify_currentShoe++;
					identify_findNextShoe = true;
				}
			}
		}

		if (state == PAIR) {
			for (int i = 0; i < blobs.size(); i++) {
				if ((blobs.at(i)->lastTimeSeenAlive + blob::timeout) < ofGetSystemTimeMillis())
				{
					cout << blobs.at(i)->lastTimeSeenAlive << endl;
					cout << blob::timeout << endl;
					cout << ofGetSystemTimeMillis() << endl;
					cout << "delete blob #" << blobs.at(i)->id << endl << endl;
					blobs.erase(blobs.begin() + i);
				}
			}
		}
	}

	// check for osc messages
	while (oscReceiver.hasWaitingMessages())
	{
		ofxOscMessage m;
		oscReceiver.getNextMessage(m);
		cout << "incoming message: " << endl << m.getAddress() << " - " << m.getArgAsString(0) << " - " << m.getRemoteIp() << endl;
		if (m.getAddress() == "/pair/accept") {
			// add shoe
			shoes.push_back(make_shared<shoe>(m.getRemoteIp(), oscSenderPort, m.getArgAsString(0)));
			shoes.back()->startOSC();
			setupShoeGUI();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetHexColor(0xffffff);

	colorImg.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	//contourFinder.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	// draw xy-coordinates at all blobs
	//for (int i = 0; i < contourFinder.nBlobs; i++) {
	//	ofRectangle r = contourFinder.blobs.at(i).boundingRect;
	//	stringstream ss;
	//	ss << contourFinder.blobs.at(i).centroid.x << "\n" << contourFinder.blobs.at(i).centroid.y;
	//	ofSetColor(255, 0, 0);
	//	ofDrawBitmapString(ss.str(), contourFinder.blobs.at(i).centroid.x + 20, contourFinder.blobs.at(i).centroid.y);
	//}

	// draw all Blobs aka Shoes
	for each (shared_ptr<blob> b in blobs) {
		b->draw(minArea, maxArea, nConsidered);
		ofNoFill();
		ofColor(0, 0, 255);
		ofCircle(b->position.x, b->position.y, threshold_blobRange);
	}

	stringstream ss;
	ss << stateStrings[state] << endl;
	ss << "contourBlobs: ";
	ss << contourFinder.nBlobs << "\n";
	ss << "particleBlobs: ";
	ss << blobs.size();
	ofSetColor(255, 0, 0);
	ofDrawBitmapString(ss.str(), 10, 10);

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------

void ofApp::scanForShoes(bool &status)
{
	if (status) {
		state = PAIR;
		shoes.clear();

		ofxOscSender oscSender;

		size_t lastDot = ipAddress.toString().find_last_of(".");
		string broadcast_ip = ipAddress.toString().substr(0, lastDot) + ".255";
		oscSender.setup(broadcast_ip, oscSenderPort);

		ofxOscMessage m;
		m.setAddress("/pair/request");
		m.addStringArg(ipAddress.toString());
		oscSender.sendMessage(m, false);

		search = false;
	}
}

//--------------------------------------------------------------

void ofApp::identifyShoes(bool &status)
{
	if (status) {
		state = IDENTIFY;
		for each(shared_ptr<shoe> shoe in shoes) {
			shoe->LEDStatus = false;
			//shoe->pairedBlob = nullptr;
			identify_currentShoe = 0;
		}
		blobs.clear();
		blob::ID_Counter = 0;
		identify_findNextShoe = true;
		identify = false;
	}
}

//--------------------------------------------------------------
// reset the Gui and add all shoes
void ofApp::setupShoeGUI()
{
	guiParameter.remove(guiShoes);
	// clear Shoes-GUI
	guiShoes.clear();
	guiShoes.setName("Shoes");
	guiShoes.add(search.set("Search for shoes", false));
	guiShoes.add(identify.set("Identify shoes", false));

	// rebuild GUI
	for each (shared_ptr<shoe> s in shoes)
		guiShoes.add(s->gui);

	guiParameter.add(guiShoes);
	gui.setup(guiParameter);
}

