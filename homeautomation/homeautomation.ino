/* Code to run on the Server Arduino Board.
 *  This code will redirect the command code issued by Bella to the appropriate
 * child node.
 *
 *  Commands:
 *  RL1O - Room Light 1 On
 *  RL1F - Room Light 2 On
 *  RL2O - Room Light 1 On
 *  RL2F - Room Light 2 On
 *  RLS - Room Lights Status
 *  KS - Kitchen Status
 *  GSS - Garden status; return the moisture
 *  GSO - Garden sprinkler turned ON
 *  GSF - Garden sprinkler turned OFF
 *  FT - Fan turned On
 *  FF - Fan turned Off
 *
 *
 *  P.S. All commands are followed by ':' character to mark Command Termination
 *
 *  Make sure to set the board as "Arduino/Genuino Mega 2560" before compiling.
 */

#include <Servo.h>
// Pin 10 and 11 for Relay
#define BAUD_RATE 9600
#define light1 10
#define light2 11

// Pins 2-7 for Ultrasonic Sesnors
#define tp1 2
#define ep1 3
#define tp2 4
#define ep2 5
#define tp3 6
#define ep3 7
#define potPosition 60
#define wateringTime 40
#define triggerMiostureContent 200
#define warningMoistureContent 750
#define delimiter ':' // To state the end of the command

// Variable to accumulate command and information
String cmd = "", stat = "";

// For each character
char ch;

// For the food inventory
String item1, item2, item3;

// Flags
boolean cmdAvailable = false;
boolean flag1 = false;
boolean flag2 = false;
boolean flag3 = false;
boolean flag4 = false;

// Define the pin for moisture sensor
const int moisturePin = A0;

// The value that which will be read from the moisture sensor
int moistureValue = 0;

// The average moisture value
float moistureAvg = 0.0;

// Pins to use the Sprinkler Motor
const int motor1Pin1 = 32;
const int motor1Pin2 = 33;
const int motor1En = 34;
const int motor2pin1 = 35;
const int motor2pin2 = 36;
const int motor2En = 37;

// Pins to use Fan Motor

// The Arduino's LED pin
const int ledPin = 13; // pin that turns on the LED

Servo servo1;

// Functions
void garden(String cmd);
void getStatus();

long microToCms(long microseconds) { return microseconds / 29 / 2; }

void setup() {
  Serial.begin(BAUD_RATE);
  Serial1.begin(BAUD_RATE);
  Serial.println("Wassup Bella!!!!!");
  // Serial1.println("Wassup Bella!!!!!");
  setupFunc();
}

void loop() {
  // Get the command from  Bella
  if (Serial1.available() > 0) {
    cmd = "";
    while (Serial1.available()) {
      // Serial.println("1");
      // Serial.flush();
      ch = Serial1.read();
      delay(5);
      if (ch == delimiter) {
        cmdAvailable = true;
        // cmd += ch;
        break;
      } else {
        cmd += ch;
      }
      delay(5);
    }
    Serial.println(cmd);
    Serial.flush();
  }
  if (cmdAvailable) {
    // Getting the work done
    // Room kitchen
    if (cmd.equals("RL1O") || cmd.equals("RL1F") || cmd.equals("RL2O") ||
        cmd.equals("RL2F") || cmd.equals("KS") || cmd.equals("FT") ||
        cmd.equals("FF")) {
      roomKitchen(cmd);
      cmdAvailable = false;
    }
    // Garden
    else if (cmd.equals("GSS") || cmd.equals("GSO") || cmd.equals("GSF")) {
      garden(cmd);
      cmdAvailable = false;
    }
    // Get status
    else if (cmd.equals("Z")) {
      getStatus();
      cmdAvailable = false;
    } else if (cmd.equals("X")) {
      turnOff();
      cmdAvailable = false;
    } else {
      Serial.println(cmd);
      Serial.flush();
      cmdAvailable = false;
    }
  }
} // Void loop

void setupFunc() {
  // Setting relay pins as Output
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);

  // Setting triggerpins as Output
  pinMode(tp1, OUTPUT);
  pinMode(tp2, OUTPUT);
  pinMode(tp3, OUTPUT);

  // Setting echopins as Input
  pinMode(ep1, INPUT);
  pinMode(ep2, INPUT);
  pinMode(ep3, INPUT);
  // Attach servo
  servo1.attach(18);
  // Motor pins
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1En, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(motor2En, OUTPUT);

  // Moisture Level Sensor
  pinMode(moisturePin, INPUT);

  digitalWrite(light1, 1);
  digitalWrite(light2, 1);
  digitalWrite(light1, 0);
  digitalWrite(light2, 0);
  digitalWrite(light1, 1);
  digitalWrite(light2, 1);
}

void roomKitchen(String cmd) {
  // Turn on Room 1 Light
  if (cmd.equals("RL1O")) {
    if (flag1) {
      Serial1.println("F1:"); // light is already on
      Serial1.flush();
      Serial.println("F1:");
      Serial.flush();
    } else {
      flag1 = true; // Change flag status
      // Relay Instruction
      digitalWrite(light1, 0);
      Serial.println("T1:");
      Serial.flush();
      Serial1.println("T1:");
      Serial1.flush();
    }
  }
  // Turn off Room Light 1
  if (cmd.equals("RL1F")) {
    if (!flag1) { // light 1 is already off
      Serial.println("F4:");
      Serial.flush();
      Serial1.println("F4:");
      Serial1.flush();
    } else {
      Serial.println("T4:");
      Serial.flush();
      flag1 = false;
      // Relay Instruction
      digitalWrite(light1, 1);
      Serial1.println("T4:");
      Serial1.flush();
    }
  }
  // Turn on Room Light 2
  if (cmd.equals("RL2O")) {
    if (flag2) { // light 2 is already on
      Serial.println("F2:");
      Serial.flush();
      Serial1.println("F2:");
      Serial1.flush();
    } else {
      Serial.println("T2:");
      Serial.flush();
      Serial1.println("T2:");
      Serial1.flush();
      flag2 = true;
      // Relay Instruction
      digitalWrite(light2, 0);
    }
  }
  // Turn off Room Light 2
  if (cmd.equals("RL2F")) {
    if (!flag2) { // light 2 is already off
      Serial.println("F5");
      Serial.flush();
      Serial1.println("F5:");
      Serial1.flush();
    } else {
      Serial.println("T5:");
      Serial.flush();
      Serial1.println("T5:");
      Serial1.flush();
      flag2 = false;
      // Relay Instruction
      digitalWrite(light2, 1);
    }
  }
  // Code for controllling the fan
  if (cmd.equals("FT")) {
    if (flag3) { // fan already on
      Serial.println("S2:");
      Serial.flush();
      Serial1.println("S2:");
      Serial1.flush();
      flag3 = true;
    } else {
      Serial.println("S1:");
      Serial.flush();
      Serial1.println("S1:");
      Serial1.flush();
      flag3 = true;
      digitalWrite(motor2En, HIGH);
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);
      delay(100);
    }
  }
  if (cmd.equals("FF")) {
    if (!flag3) { // fan already off
      Serial.println("S4:");
      Serial.flush();
      Serial1.println("S4:");
      Serial1.flush();
      flag3 = false;
    } else {
      Serial.println("S3:");
      Serial.flush();
      Serial1.println("S3:");
      Serial1.flush();
      flag3 = false;
      digitalWrite(motor2En, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, LOW);
      delay(100);
    }
  }
  if (cmd == "KS") {
    float d1, d2, d3, cm1, cm2, cm3;
    float len = 16.0, a = 0.0; // Length of box.
    int p1, p2;                // To calculate percentage of grocery

    digitalWrite(tp1, LOW); // low pulse first to ensure a clean high pulse.
    delayMicroseconds(2);
    digitalWrite(tp1, HIGH);
    delayMicroseconds(10);
    digitalWrite(tp1, LOW);
    delayMicroseconds(10);

    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.

    d1 = pulseIn(ep1, HIGH);
    // convert the time into a distance
    cm1 = microToCms(d1);
    Serial.println(cm1);
    // calculate percentage
    a = cm1 / len;
    p1 = a * 100;
    p1 = 100 - p1;
    // delay(100);
    // Serial.println(p1);

    digitalWrite(tp2, LOW); // low pulse first to ensure a clean high pulse.
    delayMicroseconds(2);
    digitalWrite(tp2, HIGH);
    delayMicroseconds(10);
    digitalWrite(tp2, LOW);
    delayMicroseconds(10);

    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.

    d2 = pulseIn(ep2, HIGH);

    // convert the time into a distance
    cm2 = microToCms(d2);
    // calculate percentage
    a = cm2 / len;
    p2 = a * 100;
    p2 = 100 - p2;
    // delay(100);

    // Send the recorded information
    if (p1 < 10) {
      Serial.print("C10");
      Serial.print(p1);
      // Serial.print(":");
      Serial.flush();
      Serial1.print("C10");
      Serial1.print(p1);
      Serial1.print("F");
      Serial1.flush();
    } else {
      Serial.print("C1");
      Serial.print(p1);
      if (p1 < 50)
        Serial.print("F");
      else
        Serial.print("T");
      Serial.flush();
      Serial1.print("C1");
      Serial1.print(p1);
      if (p1 < 50)
        Serial1.print("F");
      else
        Serial1.print("T");
      Serial1.flush();
    }

    if (p2 < 10) {
      Serial.print("C20");
      Serial.print(p2);
      Serial.println("F:");
      Serial.flush();
      Serial1.print("C20");
      Serial1.print(p2);
      Serial1.println("F:");
      Serial1.flush();
    } else {
      Serial.print("C2");
      Serial.print(p2);
      if (p2 < 50)
        Serial.println("F:");
      else
        Serial.println("T:");
      Serial.flush();
      Serial1.print("C2");
      Serial1.print(p2);
      if (p2 < 50)
        Serial1.println("F:");
      else
        Serial1.println("T:");
      Serial1.flush();
    }
    // Serial.println("C185TC234F:");
  }
}

void garden(String cmd) {
  float a;
  if (cmd.equals("GSS")) {
    a = moistureSampler();
    Serial.println(a);
    moistureAvg = (a / 1024.0) * 100.0;
    // Serial.println(a);
    // moistureAvg = a * 100;
    Serial.println(moistureAvg);
    // moistureAvg = (moistureAvg / 1024) * 100; // Calculate the percentage,
    // for dear Bella *_*
    // Serial.print(moistureAvg);
    moistureAvg = 100 - moistureAvg; // To reverse the value
    // Serial.print(moistureAvg);
    delay(10); // Just hold on a sec...
    if (moistureAvg < 10) {
      Serial1.print("M10");
      Serial1.print(moistureAvg);
      Serial1.println(":");
      Serial1.flush();
      Serial.print("M10");
      Serial.print(moistureAvg);
      Serial.println(":");
      Serial.flush();
      moistureAvg = 0; // Reset the value after printing
    } else {
      Serial.print("M1");
      Serial.print(moistureAvg);
      Serial.println(":");
      Serial.flush();
      Serial1.print("M1");
      Serial1.print(moistureAvg);
      Serial1.println(":");
      Serial1.flush();
      moistureAvg = 0; // Reset the value after printing
    }
  }
  if (cmd.equals("GSO")) {
    float moistureAvg = 0.0;
    moistureAvg = moistureSampler();
    Serial.println(moistureAvg);
    moistureAvg = (moistureAvg / 1024.0) *
                  100.0; // Calculate the percentage, for dear Bella *_*
    Serial.println(moistureAvg);
    moistureAvg = 100.0 - moistureAvg; // To reverse the value
    Serial.println(moistureAvg);
    delay(10); // Just hold on a sec...
    if (moistureAvg >= 90) {
      Serial.println("F3:"); // Soil is too wet to be watered
      Serial.flush();
      Serial1.println("F3:");
      Serial1.flush();
    } else {
      startSprinkler();
      Serial.println("T3:");
      Serial.flush();
      Serial1.println("T3:");
      Serial1.flush();
      // initPosition();
    }
    moistureAvg = 0; // Reset the value after printing
  }
  if (cmd.equals("GSF")) {
    fail();
  }
}

void startSprinkler() {
  digitalWrite(motor1En, HIGH);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  flag4 = true;
  // delay(5000);
}

void fail() {
  digitalWrite(motor1En, LOW);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  Serial.println("F7:");
  Serial.flush();
  Serial1.println("F7:");
  Serial1.flush();
  flag4 = false;
}

void initPosition() {
  servo1.write(90); // set the servo to mid-point
  delay(500);
}

int moistureSampler() {
  int sum = 0;
  for (int i = 0; i < 25; i++) {
    sum += analogRead(moisturePin);
    delay(20);
  }
  return (sum / 25);
}

void getStatus() {
  String sat = "";
  int moist, c1, c2;
  // Building the sat string

  // Satus of the room
  if (flag1)
    sat += "T";
  else
    sat += "F";
  if (flag2)
    sat += "T";
  else
    sat += "F";
  if (flag3)
    sat += "T";
  else
    sat += "F";

  // Status of the kitchen
  checkContents(&c1, &c2);
  Serial.println(c1);
  Serial.println(c2);
  if (c1 < 50)
    sat += "F";
  else
    sat += "T";
  if (c2 < 50)
    sat += "F";
  else
    sat += "T";

  // Status of the moisture in the garden
  moist = moistureSampler();
  moist = (moist / 1024) * 100;
  moist = 100 - moist;
  if (moist < 40)
    sat += "F";
  else
    sat += "T";

  // Add the delimiter
  sat += ":";

  Serial1.println(sat);
  Serial1.flush();
  Serial.println(sat);
  Serial.flush();
}

void checkContents(int *p1, int *p2) {
  float d1, d2, d3, cm1, cm2, cm3;
  int len = 16; // Length of Box. Assumed value for now. Update after boxes are
                // obtained
  float a = 0.0;
  digitalWrite(tp1, LOW); // low pulse first to ensure a clean high pulse.
  delayMicroseconds(2);
  digitalWrite(tp1, HIGH);
  delayMicroseconds(10);
  digitalWrite(tp1, LOW);
  delayMicroseconds(10);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  d1 = pulseIn(ep1, HIGH);
  // convert the time into a distance
  cm1 = microToCms(d1);
  // calculate percentage
  a = cm1 / len;
  *p1 = 100 - a * 100;
  // delay(100);

  digitalWrite(tp2, LOW); // low pulse first to ensure a clean high pulse.
  delayMicroseconds(2);
  digitalWrite(tp2, HIGH);
  delayMicroseconds(10);
  digitalWrite(tp2, LOW);
  delayMicroseconds(10);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  d2 = pulseIn(ep2, HIGH);

  // convert the time into a distance
  cm2 = microToCms(d2);
  // calculate percentage
  a = cm2 / len;
  *p2 = 100 - a * 100;
  Serial.println(*p1);
  Serial.println(*p2);
}

void turnOff() {
  int c = 0;
  if (flag1) {
    digitalWrite(light1, 1);
    flag1 = false;
    c += 1;
  }
  if (flag2) {
    digitalWrite(light2, 1);
    flag2 = false;
    c += 1;
  }
  if (flag3) {
    digitalWrite(motor2En, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
    flag3 = false;
    c += 1;
  }
  if (c > 0) {
    Serial1.println("ZT:");
    Serial1.flush();
    Serial.println("ZT:");
    Serial.flush();
    cmdAvailable = false;
  } else {
    Serial1.println("ZF:");
    Serial1.flush();
    Serial.println("ZT:");
    Serial.flush();
    cmdAvailable = false;
  }
}
