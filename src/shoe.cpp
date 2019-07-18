#include "shoe.h"
//--------------------------------------------------------------

shoe::shoe(string _ip, int _port, string _name)
{
	name = _name;

	// ---------------------------------
	// GUI
	// ---------------------------------
	LEDStatus.addListener(this, &shoe::SetLED);
	gui.setName(_name);
	gui.add(ipAddress.set("IP", _ip));
	gui.add(outPort.set("Port", to_string(_port)));
	gui.add(LEDStatus.set("LED", false));
}

//--------------------------------------------------------------

shoe::~shoe(){}

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
