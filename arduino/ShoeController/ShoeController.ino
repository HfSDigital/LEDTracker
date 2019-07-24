/*

  - reset WiFi SSID and Password: press RST-Button shortly, release and press FLASH-Button until
    the onboard-LED blinks fast. This will erase EEPROM.

*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include "pw.h"

#define LED_IR D1
#define LED_ESP D4

//-------------------------------------------------------------
// these things needed to be changed
const char shoeName[] = "Anna";
//const char shoeName[] = "Bernd";
//const char shoeName[] = "Clara";
//const char shoeName[] = "Dieter";

//-------------------------------------------------------------

unsigned long previousMillis = 0;
const long statusSendInterval = 1000;

IPAddress outIp;
WiFiUDP Udp;
const unsigned int localPort = 8000;
const unsigned int destPort = 9000;

//-------------------------------------------------------------

void setup() {
	Serial.begin(115200);

	pinMode(LED_IR, OUTPUT);        // IR LED      - D1, LED_IR
	pinMode(LED_ESP, OUTPUT);       // ESP8266 LED - 2, D4, LED_ESP
	pinMode(LED_BUILTIN, OUTPUT);   // NodeMCU LED - 16, D0, LED_BUILTIN, BUILTIN_LED

	digitalWrite(LED_IR, LOW);
	digitalWrite(LED_ESP, HIGH);
	digitalWrite(LED_BUILTIN, HIGH);

	// reset EEPROM?
	// -------------
	pinMode(D3, INPUT_PULLUP); // read FLASH-BUTTON
	delay(1000);
	if (!digitalRead(D3)) {     // Reset EEPROM
		Serial.println("Resetting EEPROM");
		for (int i = 0; i < 10; i++) {
			digitalWrite(LED_BUILTIN, LOW);
			delay(30);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(30);
		}
	}

	// Connect to WiFi
	// ---------------
	Serial.println("");
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Udp.begin(localPort);
	Serial.println();
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("Listening at port: ");
	Serial.println(Udp.localPort());
}

//-------------------------------------------------------------

void loop() {
	OSCMessage msgIN;
	int size;
	if ((size = Udp.parsePacket()) > 0)
	{
		while (size--) {
			msgIN.fill(Udp.read());
		}
		if (!msgIN.hasError()) {
			msgIN.route("/1/toggleLED", toggleOnOff);
			msgIN.route("/pair/request", pair);
		}
	}
}

//-------------------------------------------------------------

void toggleOnOff(OSCMessage &msg, int addrOffset) {
	int ledState = (boolean) msg.getInt(0);
	Serial.print("LED_State: ");
	Serial.println(ledState);
	digitalWrite(LED_BUILTIN, !ledState);
	digitalWrite(LED_IR, ledState);
}

/*-------------------------------------------------------------
void pair(OSCMessage &msg, int addrOffset);
/*-------------------------------------------------------------
This function is called when the remote computer wants to pair. For that,
it needs to broadcast an OSC-Message to the address /pair/request

The OSC-Message should contain the remote IP as a string in the format 
"192.168.12.123" (for example). If the IP gets parsed correctly, the
client now knows the remote computers IP and can answer. 

The answer goes to the address /pair/accept and contains the 
human readable name of the shoe as a string in the first argument. 
/*-------------------------------------------------------------*/
void pair(OSCMessage &msg, int addrOffset) {
	Serial.println("pair");
	if (msg.isString(0)) {
		int length = msg.getDataLength(0);
		char remoteIP[length];
		msg.getString(0, remoteIP, length);
	  
		if (!outIp.fromString(remoteIP))
		{
			Serial.println("can't parse IP-Address. Pairing failed."); 
			return;
		}

		OSCMessage msg("/pair/accept");
		msg.add(shoeName);
		Udp.beginPacket(outIp, destPort);
		msg.send(Udp);
		Udp.endPacket();
		msg.empty();
	}
}