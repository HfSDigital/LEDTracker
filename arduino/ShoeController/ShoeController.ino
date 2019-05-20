#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include "pw.h"

//-------------------------------------------------------------
// these things needed to be changed
const IPAddress outIp(192,168,235,54);        // remote IP of your computer
const char shoeName[] = "Berta";

//-------------------------------------------------------------

unsigned long previousMillis = 0;
const long statusSendInterval = 1000;

WiFiUDP Udp;
const unsigned int localPort = 8000;
const unsigned int destPort = 9000;

unsigned int ledState = 1;

//-------------------------------------------------------------

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
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
    Serial.println("msg received");
    while (size--) {
      msgIN.fill(Udp.read());
    }
    if (!msgIN.hasError()) {
      msgIN.route("/1/toggleLED", toggleOnOff);
      msgIN.route("/pair/request", pair);
    }
  }

/*
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= statusSendInterval) {
    previousMillis = currentMillis;
    Serial.print(" - AP IP address: ");
    Serial.println(WiFi.localIP());
  }
*/
}

//-------------------------------------------------------------

void toggleOnOff(OSCMessage &msg, int addrOffset) {
  Serial.println("toggle");
  ledState = (boolean) msg.getInt(0);
  Serial.println(ledState);
  digitalWrite(LED_BUILTIN, !ledState);

  if (ledState) {
    Serial.println("LED on");
  }
  else {
    Serial.println("LED off");
  }
  ledState = !ledState; // toggle the state from HIGH to LOW to HIGH to LOW ...
}

//-------------------------------------------------------------

void pair(OSCMessage &msg, int addrOffset) {
  Serial.println("pair");
  int m = msg.getInt(0);
  Serial.println(m);
  if (m == 1) {
    OSCMessage msg("/pair/accept");
    msg.add(shoeName);
    Udp.beginPacket(outIp, destPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
  }
}
