#include "shoe.h"
#include "blob.h"
//--------------------------------------------------------------

shared_ptr<shoe> shoe::shoeInPairingMode = nullptr;

shoe::shoe(string _ip, int _port, string _name)
{
	name = _name;

	pairedBlob = nullptr;

	// ---------------------------------
	// GUI
	// ---------------------------------
	LEDStatus.addListener(this, &shoe::SetLED);
	gui.setName(_name);
	gui.add(ipAddress.set("IP", _ip));
	gui.add(outPort.set("Port", to_string(_port)));
	gui.add(LEDStatus.set("LED", false));
	gui.add(pairedBlobID.set("Blob", -1));
}

//--------------------------------------------------------------

shoe::~shoe(){}

//--------------------------------------------------------------

void shoe::update() 
{
	if (pairedBlob != nullptr) 
	{
		pairedBlobID = pairedBlob->id;
	}

}

//--------------------------------------------------------------

void shoe::startOSC()
{
	// osc
	string ip = ipAddress.get();
	int p = stoi(outPort.get());
	oscSender.setup(ip, p);
	LEDStatus = true;
}
//--------------------------------------------------------------

void shoe::SetLED(bool &status)
{
	cout << "LED " << status << endl;
	ofxOscMessage m;
	m.setAddress("/1/toggleLED");
	m.addIntArg(int(status));
	oscSender.sendMessage(m, false);
}

void shoe::setLED(bool status)
{
	if (LEDStatus != status) 
	{
		LEDStatus = status;
	}
}


/* 
firstStep() is called when the shoe and blob gets paired.
It should move a few centimeters so the blob can calculate 
the direction vector.
*/
void shoe::firstStep(int durationInMillis, int speed)
{
	cout << "firstStep()" << endl;
	ofxOscMessage m;
	m.setAddress("/1/firstStep");
	m.addIntArg(int(durationInMillis));
	m.addIntArg(int(speed));
	oscSender.sendMessage(m, false);
}