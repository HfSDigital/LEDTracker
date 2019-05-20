#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include "pw.h"

//-------------------------------------------------------------
// PINS
const int MOTOR_A_SPEED = D2;
const int MOTOR_A_DIR2 = D3;
const int MOTOR_A_DIR1 = D4;
const int STBY = D5;
const int MOTOR_B_DIR1 = D6;
const int MOTOR_B_DIR2 = D7;
const int MOTOR_B_SPEED = D8;

//-------------------------------------------------------------
// UDP / OSC
WiFiUDP Udp;
const unsigned int localPort = 8000;
//const unsigned int destPort = 9000;


//-------------------------------------------------------------
// SETUP
//-------------------------------------------------------------
void setup() {
  pinMode(MOTOR_A_SPEED, OUTPUT);
  pinMode(MOTOR_A_DIR2, OUTPUT);
  pinMode(MOTOR_A_DIR1, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(MOTOR_B_DIR1, OUTPUT);
  pinMode(MOTOR_B_DIR2, OUTPUT);
  pinMode(MOTOR_B_SPEED, OUTPUT);

  digitalWrite(STBY, LOW);


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
// LOOP
//-------------------------------------------------------------
void loop() {
  // parse OSC
  OSCMessage msgIN;
  int size;
  if ((size = Udp.parsePacket()) > 0)
  {
    while (size--) {
      msgIN.fill(Udp.read());
    }
    if (!msgIN.hasError()) {
      msgIN.route("/run", setMotorSpeed);
    }
  }

}

void setMotorSpeed(OSCMessage &msg, int addrOffset) {
  float m = msg.getFloat(0);
  Serial.print(m);
  Serial.print("\t\t");
  if (m == 0) {
    digitalWrite(STBY, LOW);
  }
  else {
    digitalWrite(STBY, HIGH);

    bool motorDirection = m > 0 ? HIGH : LOW;
    digitalWrite(MOTOR_A_DIR1, motorDirection);
    digitalWrite(MOTOR_A_DIR2, !motorDirection);
    digitalWrite(MOTOR_B_DIR1, motorDirection);
    digitalWrite(MOTOR_B_DIR2, !motorDirection);

    Serial.print(m);
    Serial.print("\t\t");
    m = absf(m);            // the abs-function is broken on floats, so we do it that way...
    Serial.print(m);
    Serial.print("\t\t");
    int motorSpeed = mapf(m, 0.0, 1.0, 0.0, 255.0);     // same with map...
    analogWrite(MOTOR_A_SPEED, motorSpeed);
    analogWrite(MOTOR_B_SPEED, motorSpeed);
    Serial.println(motorSpeed);
  }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float absf(float val)
{
  return val > 0 ? val : val * -1;
}
