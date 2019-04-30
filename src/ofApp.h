#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "blob.h"


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

		void LEDTogglePressed(bool &status);

		
		// camera
		ofVideoGrabber vidGrabber;

		// tracker
		ofxCvColorImage colorImg;
		ofxCvGrayscaleImage grayImage, grayBg, grayDiff;
		ofxCvContourFinder contourFinder;

		// blobs
		vector<shared_ptr<blob>> blobs;

		// gui
		ofxPanel gui;
		ofParameterGroup guiParameter;
		
		ofParameterGroup guiTracking;
		ofParameter<float> threshold;
		ofParameter<float> threshold_blobRange;
		
		ofParameterGroup guiOSC;
		ofParameter<string> ipAddress;
		ofParameter<string> outPort;
		ofParameter<bool> LEDStatus;

		// osc
		ofxOscSender oscSender;
};
