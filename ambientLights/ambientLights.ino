#include<Adafruit_NeoPixel.h>
#define rgb 12

#define LDR A0

#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#define BAUD_RATE 9600

// EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.43.233"
const char* ssid = "CIA_Surveillance_Drone";
const char* password = "123454321";
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
bool flag = false;
char msg[100];
String cmd = "";
char delimiter = ':';
int value = 0;

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(10, rgb, NEO_GRB+NEO_KHZ800);
uint32_t red = pixels.Color(255,0,0);
uint32_t green = pixels.Color(0,255,0);
uint32_t blue = pixels.Color(0,0,255);
uint32_t white = pixels.Color(255,255,255);
uint32_t black = pixels.Color(0,0,0);

void goBlack() {
   for(int i=0;i<10;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, black); // Moderately bright green color.
    pixels.show();
  }  
}

void redShow() {
  for(int i=0;i<10;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, red); // Moderately bright green color.
  }
  pixels.show();
}

void greenShow() {
  for(int i=0;i<10;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, green); // Moderately bright green color.
    pixels.show();
  }
}

void blueShow() {
  for(int i=0;i<10;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, blue); // Moderately bright green color.
    pixels.show();
  }
}


void whiteShow(int brightness) {
  for(int i=0;i<10;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, white); // Moderately bright green color.
    pixels.setBrightness(brightness);
    pixels.show();
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

void setup() {
  Serial.begin(BAUD_RATE);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  reconnect();

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(200);
  
  pinMode(A0, INPUT);
  pixels.begin();
  goBlack();
  delay(10);
  
}

void loop() {
  // Reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3)
    reconnect();
   Serial.println(flag);
    
  if(flag) {
   value = analogRead(LDR);
   Serial.println(value);
   if(value < 100)
    whiteShow(255);
   else if(value > 850)
    whiteShow(0);
   else
   {
    value = 1024 - value;
    value /= 4;
    whiteShow(value);
   }
  }
  else
    goBlack();
  client.loop();
  // MUST delay to allow ESP8266 WIFI functions to run
  delay(10);
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert topic to string to make it easier to work with
  String topicStr = topic;
  // We expect a reply 
  cmd = String((char *)payload).substring(0,length);
  if(cmd == "AMO:")
    flag = true;
  else if(cmd == "AMF:")
    flag = false;
  else {}
   // To prevent the watch dog timer for running out...
   yield();
}


