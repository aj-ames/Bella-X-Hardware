#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.0.101"
const char* ssid = "C-10";
const char* password = "qazwsxedc@12345";
const char* mqtt_username = "astr1x";
const char* mqtt_password = "astr1x2096";

// Topic to subscribe to for the commands
char* subTopic = "bellax/req";

// Topic to publish to confirm that the light has been turned on for the python script to log
char* pubTopic = "bellax/ack";

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
  // Start the serial line for debugging
  Serial.begin(115200);
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

  // Print out some debugging info
  //Serial.println("Callback update.");
  //Serial.print("Topic: ");
  //Serial.print(topicStr);
  for (int c = 0; c < length; c++)
    Serial.write(payload[c]);
  Serial.println();
  //turn the light on if the payload is '1' and publish to the confirmation topic so the python script can log it
  if(payload[0] == '1') {
    //digitalWrite(lightPin, HIGH);
    client.publish(pubTopic, "On");
  }

  //turn the light off if the payload is '0' and publish to the confirmation topic so the python script can log it
  else if (payload[0] == '0') {
    //digitalWrite(lightPin, LOW);
    client.publish(pubTopic, "Off");
  }
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
  // Serial.println("");
  // Serial.println("WiFi connected");  
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
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

