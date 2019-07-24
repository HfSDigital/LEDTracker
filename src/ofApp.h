#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxNetworkUtils.h"

#include "blob.h"
#include "shoe.h"


class progressBar
{
public:
	progressBar();
	void update();
	void draw();
	void init(string _name, uint64_t _time);

	string name;
	float percentage;
	ofVec2f size;
	ofVec2f pos;
	bool b_visible;
	uint64_t startTime;
	uint64_t duration;
};



class ofApp : public ofBaseApp
{
private:
	enum States { IDLE, RECEIVEOSC, PAIRING, ALLLEDSOFF, CHECKSHOES, TURNONONESHOE, PAIRSHOE, ALLLEDSON };
	string stateStrings[8] = { "IDLE", "RECEIVEOSC", "PAIRING", "ALLLEDSOFF", "CHECKSHOES", "TURNONONESHOE", "PAIRSHOE", "ALLLEDSON" };
	
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void scanForShoes(bool &status);
	void setupShoeGUI();
	void switchState(States _state);
	void switchState(States _state, uint64_t _duration);
		
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
	States state;
	uint64_t stateDuration = 5000;
	uint64_t lastStateSwitch;
	progressBar pBar;

	// ---------------------------------
	// BLOBS AND SHOES
	// ---------------------------------
	vector<shared_ptr<blob>> blobs;
	vector<shared_ptr<shoe>> shoes;	// the osc-connected shoes

};

