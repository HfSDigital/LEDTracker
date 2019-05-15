#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "blob.h"
#include "shoe.h"

class ofApp : public ofBaseApp{

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
		void setupGui();
		
		// camera
		ofVideoGrabber vidGrabber;

		// tracker
		ofxCvColorImage colorImg;
		ofxCvGrayscaleImage grayImage, grayBg, grayDiff;
		ofxCvContourFinder contourFinder;

		// blobs and shoes
		vector<shared_ptr<blob>> blobs;
		vector<shared_ptr<shoe>> shoes;

		// gui
		ofxPanel gui;
		ofParameterGroup guiParameter;
		
		ofParameterGroup guiTracking;
		ofParameter<float> threshold;
		ofParameter<float> threshold_blobRange;

		ofParameterGroup guiShoes;
		ofParameter<bool> search = false;
		//ofxButton search;
		
		
		// osc
		//ofxOscSender oscSender;
		// listening port
		const int oscSenderPort = 8000;
		const int oscReceiverPort = 9000;
		ofxOscReceiver oscReceiver;
};
