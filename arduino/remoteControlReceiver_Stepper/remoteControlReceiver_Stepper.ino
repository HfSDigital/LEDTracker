#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include "pw.h"

//-------------------------------------------------------------
// PINS
const int MOTOR_A_STEP = D4;
const int MOTOR_A_DIR = D3;

const int MOTOR_B_STEP = D5;
const int MOTOR_B_DIR = D6;

//-------------------------------------------------------------
// UDP / OSC
WiFiUDP Udp;
const unsigned int localPort = 8000;
//const unsigned int destPort = 9000;


//-------------------------------------------------------------
// GLOBAL VARIABLES
bool runMotors = false;
bool stepSlope = LOW;
bool motorDirection = HIGH;
float stepTime = 1000;
unsigned long previousMicros = 0;

//-------------------------------------------------------------
// SETUP
//-------------------------------------------------------------
void setup() {

  pinMode(MOTOR_A_STEP, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_B_STEP, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);

  digitalWrite(MOTOR_A_DIR, motorDirection);
  digitalWrite(MOTOR_B_DIR, motorDirection);


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

  if (runMotors) {
    // do the steps
    unsigned long currentMicros = micros();
    if (currentMicros - previousMicros >= stepTime) {
      previousMicros = currentMicros;
      digitalWrite(MOTOR_A_STEP , stepSlope);
      digitalWrite(MOTOR_B_STEP , stepSlope);
      stepSlope = !stepSlope;
    }
  }
}

//-------------------------------------------------------------

void setMotorSpeed(OSCMessage &msg, int addrOffset) {
  float m = msg.getFloat(0);
  Serial.println();
  Serial.print(m);
  Serial.print("\t\t");
  if (m == 0) {
    runMotors = false;
  }
  else {
    runMotors = true;
    motorDirection = m > 0 ? HIGH : LOW;
    digitalWrite(MOTOR_A_DIR, motorDirection);
    digitalWrite(MOTOR_B_DIR, motorDirection);

    Serial.print(m);
    Serial.print("\t\t");
    m = absf(m); // the abs-function is broken on floats, so we do it that way...
    Serial.print(m);
    Serial.print("\t\t");
    stepTime = mapf(m, 0.0, 1.0, 2000.0, 100.0);  // same with map...
    Serial.print(stepTime);
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
