/*
 * To get everything working, do the following:
 * Step un: First connect just the ESP and come out of the loop. This is because of theretained message 
 * Step deux: Connect the TX(ESP) --> RX(Mega) and RX(ESP) --> TX(Mega) 
 * Step trois: No such thing now everything works! Bien joué  
 */

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#define BAUD_RATE 9600


// EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.0.34"
const char* ssid = "Bubble";
const char* password = "Bubble101";
const char* mqtt_username = "astr1x";
const char* mqtt_password = "astr1x2096";

// Topic to subscribe to for the commands
char* subTopic = "bellax/req";

// Topic to publish to confirm that the light has been turned on for the python script to log
char* pubTopic = "bellax/ack";

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

// Global variable
bool received = false;
char msg[100];
String ack = "";
char ch;
char delimiter = ':';

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
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
  // We expect a reply 
  received = false;
  // Send it to MEGA 
  Serial.write(payload, length);
  Serial.println();
  Serial.flush();

  while (!received) {
    //Serial.println("Wating..");
    if (Serial.available() > 0) {
      ack = "";
      while (Serial.available()) {
        ch = Serial.read();
        delay(5);
        if (ch == delimiter) {
          received = true;
          break;
        } else {
          ack += ch;
        }
     }
    }
   // To prevent the watch dog timer for running out...
   yield();
  }

  if (received) {
    ack.toCharArray(msg, ack.length() + 1);
    client.publish(pubTopic, msg);
  }
}


//networking functions

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

