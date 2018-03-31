#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define BAUD_RATE 9600

// EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.43.233"
const char* ssid = "CIA_Surveillance_Drone";
const char* password = "123454321";
const char* mqtt_username = "astr1x";
const char* mqtt_password = "astr1x2096";

#include "DHT.h"

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);
#define DHTPIN 14   // Connected to Pin D5 in NodeMCU

#define LDR A0

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

float humidity = 0.0, temperature = 0.0;
int light = 0;
long startTime = 0;

// Topic to subscribe to for the commands
char* subTopic = "bellax/req";

// Topic to publish to confirm that the light has been turned on for the python script to log
char* pubTopic = "bellax/ack";

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);
 
void setup() {
  Serial.begin(BAUD_RATE); 
  pinMode(LDR, INPUT);

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

  delay(5);String cmd = "";
  
  // dht begin
  dht.begin();
  
  // set display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  
}

void showValues(float temp, float hud, int light) {
  display.setTextSize(1);
  display.setCursor(3,0);
  display.println("Temperature/Humidity");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.print(temp);
  display.println("C");
  display.setCursor(75,10);
  display.print(hud);
  display.println("%");
  display.setCursor(30, 20);
  display.println("LDR");
  display.setCursor(75, 20);
  display.println(light);
  display.display();
  display.clearDisplay();
}

void readDHT() {
  float pseudotemperature = dht.readTemperature();   // Read temperature as Celsius
  float pseudohumidity = dht.readHumidity();
  delay(100);
 
  if (isnan(humidity) || isnan(pseudotemperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  temperature = pseudotemperature;
  humidity = pseudohumidity;
}

void readLDR() {
  light = analogRead(LDR);
}
void loop() {
  // Reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3)
    reconnect();

  // Maintain MQTT connection
  client.loop();
  readDHT();
  startTime = millis();
  while(millis() - startTime < 2000) {
    client.loop();
    readLDR();
    delay(100);
    showValues(temperature, humidity, light);  
  }
  showValues(temperature, humidity, light);  
  Serial.print("Humidity: ");    // show in serial monitor
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");    // show in serial monitor
  Serial.print(temperature);
  Serial.print(" *C \n");
  Serial.print("LDR: ");    // show in serial monitor
  Serial.print(light);
  Serial.print(" units \n");
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  
  // Convert topic to string to make it easier to work with
  String topicStr = topic;
  // Get stuff working
  String cmd = String((char *)payload).substring(0,length);
  char *msg={};
  if(cmd.equals("S;T")) {
    String msg = "S;T;" + String(temperature);
    client.publish(pubTopic, msg.c_str());
  }
  if(cmd.equals("S;H")) {
    String msg = "S;H;" + String(humidity);
    client.publish(pubTopic, msg.c_str());
  }
  if(cmd.equals("S;L")) {
    String msg = "S;L;" + String(light);
    client.publish(pubTopic, msg.c_str());
  }
}

void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED) {

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
}

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "BellaX-SensorArray";

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str(), mqtt_username, mqtt_password)) {
        //Serial.print("\tMTQQ Connected");
        client.subscribe(subTopic);
      }
    }
  }
}
