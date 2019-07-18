#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
//#include "blob.h"

class shoe {
private:
	void SetLED(bool &status);

public:
	shoe(string _ip = "localhost", int _port = 4444, string _name ="noname");
	~shoe();
	void startOSC();

	//--------------------------------------------------------------
	string name;
	//shared_ptr<blob> pairedBlob;
	// ---------------------------------
	// OSC
	// ---------------------------------
	ofxOscSender oscSender;

	// ---------------------------------
	// GUI
	// ---------------------------------
	ofParameterGroup gui;
	ofParameter<string> ipAddress;
	ofParameter<string> outPort;
	ofParameter<bool> LEDStatus;
};