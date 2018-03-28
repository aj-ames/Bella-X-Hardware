#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#define BAUD_RATE 9600

// Sowftware serial
SoftwareSerial swSer(14, 12, false, 256);


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
char msg[5];
String ack = "";
char ch;
char delimiter = ':';

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
  // Start the serial line for debugging
  Serial.begin(BAUD_RATE);
  swSer.begin(BAUD_RATE);
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
  // Print out some debugging info
  //Serial.println("Callback update.");
  //Serial.print("Topic: ");
  //Serial.println(topicStr);
  //Serial.println("Here");
  Serial.write(payload, length);
  //Serial.flush();
  //swSer.write(payload, length);
  //swSer.flush();
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
      delay(5);
     }
     //Serial.println(ack);
     //Serial.flush();
    }
   ack.toCharArray(msg, ack.length() + 1);
   client.publish(pubTopic, msg);
   yield();
  }
  //Serial.println("Done");
}


//networking functions

void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED) {
    //debug printing
    // Serial.print("Connecting to ");
    // Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      // Serial.print(".");
  }

  //print out some more debug once connected
   //Serial.println("");
   //Serial.println("WiFi connected");  
   //Serial.println("IP address: ");
   //Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      //Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str(), mqtt_username, mqtt_password)) {
        //Serial.print("\tMTQQ Connected");
        client.subscribe(subTopic);
      }

      //otherwise print failed for debugging
      else { 
        //Serial.println("\tFailed."); abort();
      }
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5) {
      result += ':';
    }
  }
  return result;
}

