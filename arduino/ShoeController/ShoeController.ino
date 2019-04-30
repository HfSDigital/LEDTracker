#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include "pw.h"



unsigned long previousMillis = 0;
const long statusSendInterval = 1000;

WiFiUDP Udp;
const unsigned int localPort = 8000;
const unsigned int destPort = 9000;

unsigned int ledState = 1;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("Configuring access point...");

  WiFi.begin(ssid, password);
  Udp.begin(localPort);
}

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
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= statusSendInterval) {
    previousMillis = currentMillis;
    Serial.print(" - AP IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void toggleOnOff(OSCMessage &msg, int addrOffset) {
  Serial.println("toggle");
  ledState = (boolean) msg.getInt(0);
  Serial.println(ledState);
  digitalWrite(LED_BUILTIN, ledState);
  //digitalWrite(ledPin, ledState);               // External LED

  if (ledState) {
    Serial.println("LED on");
  }
  else {
    Serial.println("LED off");
  }
  ledState = !ledState; // toggle the state from HIGH to LOW to HIGH to LOW ...
}

