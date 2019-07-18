#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxNetworkUtils.h"

#include "blob.h"
#include "shoe.h"

class ofApp : public ofBaseApp{
	private:
		enum State { PAIR, IDENTIFY, PLAY };
		string stateStrings[3] = { "PAIR", "IDENTIFY", "PLAY" };
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void scanForShoes(bool &status);
		void identifyShoes(bool &status);
		void setupShoeGUI();
		
		// ---------------------------------
		// CAMERA + TRACKER
		// ---------------------------------
		ofVideoGrabber vidGrabber;
		ofxCvColorImage colorImg;
		ofxCvGrayscaleImage grayImage, grayBg, grayDiff;
		ofxCvContourFinder contourFinder;

		// ---------------------------------
		// GUI
		// ---------------------------------
		ofxPanel gui;
		ofParameterGroup guiParameter;
		ofParameter<string> ipAddress;

		ofParameterGroup guiTracking;
		ofParameter<float> threshold;
		ofParameter<float> threshold_blobRange;
		ofParameter<float> minArea;
		ofParameter<float> maxArea;
		ofParameter<float> nConsidered;

		ofParameterGroup guiShoes;
		ofParameter<bool> search = false;
		ofParameter<bool> identify = false;

		// ---------------------------------
		// Network Utils
		// ---------------------------------
		Poco::Net::NetworkInterface::List siteLocalInterfaces;
		
		// ---------------------------------
		// OSC 
		// ---------------------------------
		const int oscSenderPort = 8000;
		const int oscReceiverPort = 9000;
		ofxOscReceiver oscReceiver;

		// ---------------------------------
		// STATE MACHINE
		// ---------------------------------
		State state = PAIR;

		// ---------------------------------
		// BLOBS AND SHOES
		// ---------------------------------
		vector<shared_ptr<blob>> blobs;
		vector<shared_ptr<shoe>> shoes;
		map<shared_ptr<shoe>, shared_ptr<blob>> trackedShoes;

		// ---------------------------------
		// Identify
		// ---------------------------------
		unsigned int identify_currentShoe = 0;
		bool identify_findNextShoe = false;
};
