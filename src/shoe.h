#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class shoe {
public:
	shoe(string _ip = "localhost", int _port = 4444, string _name ="noname");
	~shoe();
	void startOSC();
	void LEDTogglePressed(bool &status);

	//--------------------------------------------------------------
	string name;
	ofxOscSender oscSender;

	ofParameterGroup gui;
	ofParameter<string> ipAddress;
	ofParameter<string> outPort;
	ofParameter<bool> LEDStatus;
};