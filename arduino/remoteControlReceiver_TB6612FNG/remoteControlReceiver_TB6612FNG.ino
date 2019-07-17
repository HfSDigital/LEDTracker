#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include "pw.h"

//-------------------------------------------------------------
// PINS
const int MOTOR_A_SPEED = D8;
const int MOTOR_A_DIR2 = D7;
const int MOTOR_A_DIR1 = D6;
const int STBY = D5;
const int MOTOR_B_DIR1 = D4;
const int MOTOR_B_DIR2 = D3;
const int MOTOR_B_SPEED = D2;
const int IR_LED = D1;

//-------------------------------------------------------------
// UDP / OSC
WiFiUDP Udp;
const unsigned int localPort = 8000;
//const unsigned int destPort = 9000;


//-------------------------------------------------------------
// SETUP
//-------------------------------------------------------------
void setup() {
  analogWriteFreq(40);
  pinMode(MOTOR_A_SPEED, OUTPUT);
  pinMode(MOTOR_A_DIR2, OUTPUT);
  pinMode(MOTOR_A_DIR1, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(MOTOR_B_DIR1, OUTPUT);
  pinMode(MOTOR_B_DIR2, OUTPUT);
  pinMode(MOTOR_B_SPEED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
 

  digitalWrite(STBY, LOW);
  digitalWrite(D0, HIGH);  // Turn the LED on the NodeMCU off (16, D0, LED_BUILTIN, BUILTIN_LED)
  digitalWrite(D4, HIGH);  // Turn the LED on the ESP-12 off  (2, D4)

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
  float m1 = msg.getFloat(0);
  float m2 = msg.getFloat(1);
  Serial.println();
  Serial.print("m1: ");
  Serial.print(m1);
  Serial.print("\tm2: ");
  Serial.print(m2);
  Serial.print("\t");
  if (m1 == 0) {
    digitalWrite(STBY, LOW);
  }
  else {
    digitalWrite(STBY, HIGH);

    bool m1Direction = m1 > 0 ? HIGH : LOW;
    bool m2Direction = m2 > 0 ? HIGH : LOW;
    digitalWrite(MOTOR_A_DIR1, m1Direction);
    digitalWrite(MOTOR_A_DIR2, !m1Direction);
    digitalWrite(MOTOR_B_DIR1, !m2Direction);
    digitalWrite(MOTOR_B_DIR2, m2Direction);

    m1 = absf(m1);
    m2 = absf(m2);
    int m1Speed = mapf(m1, 0.0, 1.0, 0.0, 1023.0);     // note: NodeMCU has 10-Bit AnalogWrite resolution, 
    int m2Speed = mapf(m2, 0.0, 1.0, 0.0, 1023.0);     // so we map to 1023 and not 255
    analogWrite(MOTOR_A_SPEED, m1Speed);
    analogWrite(MOTOR_B_SPEED, m2Speed);
    Serial.print("Speed: ");
    Serial.print(m1Speed);
    Serial.print("\t");
    Serial.print(m2Speed);
  }
}


// the map-function seems to be broken on floats
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// the abs-function seems to be broken on floats
float absf(float val)
{
  return val > 0 ? val : val * -1;
}
