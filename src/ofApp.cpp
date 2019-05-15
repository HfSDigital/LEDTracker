#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	blob::ID_Counter = 0;

	// camera
	vidGrabber.setVerbose(true);
	vidGrabber.setDeviceID(2);
	vidGrabber.initGrabber(1280, 720);
	vidGrabber.videoSettings();

	// tracker
	colorImg.allocate(1280, 720);
	grayImage.allocate(1280, 720);
	grayBg.allocate(1280, 720);
	grayDiff.allocate(1280, 720);

	// gui
	search.addListener(this, &ofApp::scanForShoes);

	guiParameter.setName("Settings");

	guiTracking.setName("Tracking");
	guiTracking.add(threshold.set("threshold", 30, 0, 100));
	guiTracking.add(threshold_blobRange.set("blob Range", 10, 0, 100));
	guiParameter.add(guiTracking);

	guiShoes.setName("Shoes");
	guiShoes.add(search.set("Search for shoes", false));
	guiParameter.add(guiShoes);

	gui.setup(guiParameter);
	gui.loadFromFile("settings.xml");

	oscReceiver.setup(oscReceiverPort);
}

//--------------------------------------------------------------
void ofApp::update(){
	vidGrabber.update();
	//do we have a new frame?
	if (vidGrabber.isFrameNew()) {
		colorImg.setFromPixels(vidGrabber.getPixels());
		grayImage = colorImg; // convert our color image to a grayscale image
		//if (bLearnBackground == true) {
		//	grayBg = grayImage; // update the background image
		//	bLearnBackground = false;
		//}
		//grayDiff.absDiff(grayBg, grayImage);
		//grayDiff.threshold(30);
		grayImage.threshold(threshold);
		
		contourFinder.findContours(grayImage, 5, (340 * 240) / 4, 4, false, true);
		//contourFinder.findContours(grayDiff, 5, (340 * 240) / 4, 4, false, true);

		for (int j = 0; j < contourFinder.nBlobs; j++) {
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
			}
		}

		// remove blobs with timeout
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

	// check for osc messages
	while (oscReceiver.hasWaitingMessages())
	{
		ofxOscMessage m;
		oscReceiver.getNextMessage(m);
		cout << "incoming message: " << endl << m.getAddress() << " - " << m.getArgAsString(0) << " - " << m.getRemoteIp() << endl;
		if (m.getAddress() == "/pair/accept") {
			// add shoe
			shoes.push_back(make_shared<shoe>(m.getRemoteIp(), oscSenderPort, m.getArgAsString(0)));
			//guiParameter.add(shoes.back()->gui);
			//gui.setup(guiParameter);
			shoes.back()->startOSC();
			setupGui();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetHexColor(0xffffff);

	//colorImg.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	grayImage.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	//grayDiff.draw(0, 240, 320, 240);
	//ofDrawRectangle(320, 0, 320, 240);
	//contourFinder.draw(320, 0, 320, 240);
	contourFinder.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	//ofColor c(255, 255, 255);
	for (int i = 0; i < contourFinder.nBlobs; i++) {
		ofRectangle r = contourFinder.blobs.at(i).boundingRect;
		stringstream ss;
		ss << contourFinder.blobs.at(i).centroid.x << "\n" << contourFinder.blobs.at(i).centroid.y;
		ofSetColor(255, 0, 0);
		ofDrawBitmapString(ss.str(), contourFinder.blobs.at(i).centroid.x + 20, contourFinder.blobs.at(i).centroid.y);
		/*c.setHsb(i * 64, 255, 255);
		ofSetColor(c);
		ofDrawRectangle(r);*/
	}

	for each (shared_ptr<blob> b in blobs) {
		b->draw();
		ofNoFill();
		ofColor(0, 0, 255);
		ofCircle(b->position.x, b->position.y, threshold_blobRange);
	}
	//colorImg.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	stringstream ss;
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
		shoes.clear();

		ofxOscSender oscSender;
		oscSender.setup("192.168.235.255", oscSenderPort);

		ofxOscMessage m;
		m.setAddress("/pair/request");
		m.addIntArg(1);
		oscSender.sendMessage(m, false);

		search = false;
	}
}


//--------------------------------------------------------------
// reset the Gui and add all shoes
void ofApp::setupGui()
{
	guiParameter.remove(guiShoes);
	// clear Shoes-GUI
	guiShoes.clear();
	guiShoes.setName("Shoes");
	guiShoes.add(search.set("Search for shoes", false));

	// rebuild GUI


	for each (shared_ptr<shoe> s in shoes)
		guiShoes.add(s->gui);


	guiParameter.add(guiShoes);

	gui.setup(guiParameter);



	// osc
	//for each (shared_ptr<shoe> s in shoes)
	//	s->startOSC();

}