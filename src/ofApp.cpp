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

	switchState(IDLE);
}

//--------------------------------------------------------------
void ofApp::update(){
	bool allShoesPaired = true;

	switch (state) {

	case IDLE:
	{
		blob::timeout = 3000;
		break;
	}

	// 0) check for osc messages
	case RECEIVEOSC:
	{
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

		if (ofGetElapsedTimeMillis() - lastStateSwitch > stateDuration)
			switchState(CHECKSHOES, stateDuration);
		break;
	}

	// 1) check if all shoes are paired to a blob
	case CHECKSHOES:
	{
		for each (shared_ptr<shoe> s in shoes) {
			if (s->pairedBlob == nullptr)
			{
				allShoesPaired = false;
				break;
			}
		}
		allShoesPaired ? switchState(ALLLEDSON, stateDuration / 2) : switchState(ALLLEDSOFF, stateDuration / 2);
		break;
	}

	// 2)  turn off all IR-LEDs
	case ALLLEDSOFF:
	{
		blob::timeout = 100;
		for each (shared_ptr<shoe> s in shoes) {
			s->setLED(false);
		}

		if (ofGetElapsedTimeMillis() - lastStateSwitch > stateDuration / 2)
			switchState(TURNONONESHOE, stateDuration / 2);
		break;
	}

	// 3) turn on one shoe
	case TURNONONESHOE:
	{
		for each (shared_ptr<shoe> s in shoes) {
			if (s->pairedBlob == nullptr)
			{
				s->setLED(true);
				s->shoeInPairingMode = s;
				break;
			}
		}

		if (ofGetElapsedTimeMillis() - lastStateSwitch > stateDuration / 2)
			switchState(PAIRSHOE, stateDuration / 2);
		break;
	}

	// 4) pair 'shoe in pairing mode' with the last blob that was turned on
	case PAIRSHOE:
	{
		if (blobs.size() > 0 && shoe::shoeInPairingMode->pairedBlob == nullptr) {
			cout << "pair shoe.." << endl;
			shoe::shoeInPairingMode->pairedBlob = blobs.back();
			blobs.back()->pairedShoe = shoe::shoeInPairingMode;
			shoe::shoeInPairingMode->firstStep();
		}
		if (ofGetElapsedTimeMillis() - lastStateSwitch > stateDuration / 2)
			switchState(CHECKSHOES);
		break;
	}

	// 5) turn on all IR-LEDs
	case ALLLEDSON:
	{
		for each (shared_ptr<shoe> s in shoes) {
			s->setLED(true);
		}

		if (ofGetElapsedTimeMillis() - lastStateSwitch > stateDuration / 2)
			switchState(IDLE);
		break;
	}

	default:
		break;
	}
	

	// UPDATES...
	vidGrabber.update();
	pBar.update();
	for each(shared_ptr<shoe> s in shoes) 
	{
		s->update();
	}
	for each(shared_ptr<blob> b in blobs)
	{
		b->update(ofGetMouseX(), ofGetMouseY(), ofGetMousePressed(), threshold_blobRange);
	}

	
	
	// TRACKING AND BLOB DETECTION
	// do we have a new frame? -> do the tracking!
	if (vidGrabber.isFrameNew()) {
		colorImg.setFromPixels(vidGrabber.getPixels());
		grayImage = colorImg;			// convert our color image to a grayscale image..
		grayImage.threshold(threshold); // add a threshold..
		contourFinder.findContours(grayImage, minArea, maxArea, nConsidered, false, true);
		for (int j = 0; j < contourFinder.blobs.size(); j++) {
			// iterate through ALL blobs the contourFinder finds
			bool blobExists = false;
			for (int i = 0; i < blobs.size(); i++) {
				// comparing the positions of all known blobs with the position of the tracked blob. Is it known?
				float dist = ofDist(contourFinder.blobs.at(j).centroid.x, contourFinder.blobs.at(j).centroid.y, blobs.at(i)->position.x, blobs.at(i)->position.y);
				if (dist < threshold_blobRange) {
					// the blob is known!! just update it's position!
					blobExists = true;
					blobs.at(i)->updatePos(contourFinder.blobs.at(j).centroid.x, contourFinder.blobs.at(j).centroid.y);
				}
			}
			if (!blobExists)
			{
				// the blob is NOT known? add it to the blobs-vector.
				float x = contourFinder.blobs.at(j).centroid.x;
				float y = contourFinder.blobs.at(j).centroid.y;
				blobs.push_back(make_shared<blob>(x, y));
			}
		}

	}

	// DELETE UNUSED BLOBS
	// If a registrated blob can't be found in the current tracking for a certain amount of time
	// AND the blob isn't paired to a shoe, delete it.
	for (int i = 0; i < blobs.size(); i++) {
		if ((blobs.at(i)->lastTimeSeenAlive + blob::timeout) < ofGetSystemTimeMillis())
		{
			bool notInShoes = true;
			for each(shared_ptr<shoe> s in shoes)
			{
				if (blobs.at(i) == s->pairedBlob) {
					notInShoes = false;
					break;
				}
			}
			if (notInShoes)
				blobs.erase(blobs.begin() + i);
		}
	}



}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetHexColor(0xffffff);

	colorImg.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	// draw all Blobs aka Shoes
	for each (shared_ptr<blob> b in blobs) {
		b->draw(minArea, maxArea, nConsidered, threshold_blobRange);
	}

	stringstream ss;
	ss << stateStrings[state] << endl;
	ss << "contourBlobs: ";
	ss << contourFinder.nBlobs << "\n";
	ss << "particleBlobs: ";
	ss << blobs.size() << "\n";

	ofSetColor(255, 0, 0);
	ofDrawBitmapString(ss.str(), 400, 10);

	gui.draw();
	pBar.draw();
}

//--------------------------------------------------------------

void ofApp::scanForShoes(bool &status)
{
	if (status) {
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

		switchState(RECEIVEOSC, stateDuration);
	}
}


void ofApp::setupShoeGUI()
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
}

void ofApp::switchState(States _state)
{
	state = _state;
	cout << endl << stateStrings[state];
}

void ofApp::switchState(States _state, uint64_t _duration)
{
	state = _state;
	cout << endl << stateStrings[state];
	if (_duration == 0) return;

	pBar.init(stateStrings[state], _duration);
	lastStateSwitch = ofGetElapsedTimeMillis();
}



void ofApp::keyPressed(int key) {
	if (key == 'a') {

	}
	else if (key == 'r')
	{
		blob::ID_Counter = 0;
		blobs.clear();
		shoes.clear();
		setupShoeGUI();
	}

}

void ofApp::keyReleased(int key) {

}

void ofApp::mousePressed(int x, int y, int button) 
{
	if (button == 0) {
		for each(shared_ptr<blob> b in blobs) {
			b->mouseClicked();
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button) 
{
}


progressBar::progressBar()
{
	size.x = ofGetWindowWidth() / 4;
	size.y = ofGetWindowHeight() / 20;
	pos.x = ofGetWindowWidth() / 2 - size.x / 2;
	pos.y = ofGetWindowHeight() / 2 - size.y / 2;
	percentage = 0.0f;
	b_visible = false;
}

void progressBar::draw()
{
	if (!b_visible) return;
	ofNoFill();
	ofSetColor(222);
	ofDrawBitmapString(name + " " + to_string(percentage), pos.x, pos.y - 5);
	ofDrawRectangle(pos, size.x, size.y);
	ofFill();
	ofDrawRectangle(pos, size.x * percentage, size.y);
}

void progressBar::update()
{
	if (!b_visible) return;
	if (percentage > 1.0) 
	{
		b_visible = false;
		return;
	}

	percentage = (float)(ofGetElapsedTimeMillis() - startTime) / (float)duration;
	cout << ".";
}

void progressBar::init(string _name, uint64_t _time)
{
	name = _name;
	duration = _time;
	b_visible = true;
	percentage = 0.0f;
	startTime = ofGetElapsedTimeMillis();
}

