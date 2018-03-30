#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define BAUD_RATE 9600

// EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.43.233"
const char* ssid = "CIA_Survrillance_Drone";
const char* password = "123454321";
const char* mqtt_username = "astr1x";
const char* mqtt_password = "astr1x2096";

// Pins
const int bulb1 = 5; // D1
const int bulb2 = 4; // D2
const int fan = 0; // D3
const int motorPin1 = 14; // D5
const int motorPin2 = 12; // D6
const int motorEn = 13; // D7


// Topic to subscribe to for the commands
char* subTopic = "bellax/req";

// Topic to publish to confirm that the light has been turned on for the python script to log
char* pubTopic = "bellax/ack";

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

// Global variable
bool stat_bulb1 = false, stat_bulb2 = false, stat_fan = false;
String cmd = "";

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setupFunc() {
    pinMode(bulb1, OUTPUT);
    pinMode(bulb2, OUTPUT);
    pinMode(fan, OUTPUT);
}


void setup() {
  setupFunc();
  // Start the serial line for debugging
  Serial.begin(BAUD_RATE);
  delay(100);

  // Start wifi subsystem
  WiFi.begin(ssid, password);

  // Attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  // Wait a bit before starting the main loop
  delay(2000);
}


void loop() {
  // Reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3)
    reconnect();

  // Maintain MQTT connection
  client.loop();
  
  // MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  
  // Convert topic to string to make it easier to work with
  String topicStr = topic;
  // Get stuff working
  cmd = String((char *)payload).substring(0,length);
  if (cmd.equals("RL1O")) {
    if (stat_bulb1) {
      // light is already on
      client.publish(pubTopic, "H:1A");
    } else {
      stat_bulb1 = true; // Change flag status
      // Relay Instruction
      digitalWrite(bulb1, 0);
      client.publish(pubTopic, "H:1T");
    }
  }
  // Turn off Room Light 1
  if (cmd.equals("RL1F")) {
    if (!stat_bulb1) { // light 1 is already off
      client.publish(pubTopic, "H:1B");
    } else {
      digitalWrite(bulb1, 1);
      stat_bulb1 = false;
      client.publish(pubTopic, "H:1F");
    }
  }
  // Turn on Room Light 2
  if (cmd.equals("RL2O")) {
    if (stat_bulb2) { // light 2 is already on
      client.publish(pubTopic, "H:2A");
    } else {
      stat_bulb2 = true;
      // Relay Instruction
      digitalWrite(bulb2, 0);
      client.publish(pubTopic, "H:2T");
    }
  }
  // Turn off Room Light 2
  if (cmd.equals("RL2F")) {
    if (!stat_bulb2) { // light 2 is already off
      client.publish(pubTopic, "H:2B");
    } else {
      stat_bulb2 = false;
      // Relay Instruction
      digitalWrite(bulb2, 1);
      client.publish(pubTopic, "H:2F");
    }
  }
  // Code for controllling the fan
  if (cmd.equals("FO")) {
    if (stat_fan) { // fan already on
      client.publish(pubTopic, "H:3A");
    } else {
      client.publish(pubTopic, "H:3T");
      stat_fan = true;
      digitalWrite(motorEn, HIGH);
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      delay(100);
    }
  }
  if (cmd.equals("FF")) {
    if (!stat_fan) { // fan already off
      client.publish(pubTopic, "H:3B");
    } else {
      client.publish(pubTopic, "H:3F");
      stat_fan = false;
      digitalWrite(motorEn, LOW);
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      delay(100);
    }
  }
}

void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED) {

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      // Serial.print(".");
    }
}

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "BellaX-Hardware";

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str(), mqtt_username, mqtt_password)) {
        //Serial.print("\tMTQQ Connected");
        client.subscribe(subTopic);
      }
    }
  }
}


