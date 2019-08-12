#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
//#include "blob.h"

class blob;

class shoe : public enable_shared_from_this<shoe> {
private:
	void SetLED(bool &status);

public:
	shoe(string _ip = "localhost", int _port = 4444, string _name ="noname");
	~shoe();
	void startOSC();
	void update();
	void setLED(bool status);
	void firstStep(int durationInMillis = 1000, int speed = 25);

	// ---------------------------------

	string name;
	shared_ptr<blob> pairedBlob;
	static shared_ptr<shoe> shoeInPairingMode;

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
	ofParameter<int> pairedBlobID;
};