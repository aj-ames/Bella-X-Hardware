#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#define BAUD_RATE 9600

// EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.43.233"
const char *ssid = "CIA_Surveillance_Drone";
const char *password = "123454321";
const char *mqtt_username = "astr1x";
const char *mqtt_password = "astr1x2096";

// Topic to subscribe to for the commands
char *subTopic = "bellax/req";

// Topic to publish to confirm that the light has been turned on for the python
// script to log
char *pubTopic = "bellax/ack";

// Callback function header
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();

// MQTT
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);
#define DHTPIN 14 // Connected to Pin D5 in NodeMCU

// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE, 15);

byte img[] = {
    0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0, 0x0,  0xf,  0xf0, 0x0,  0x0,  0x0,  0x0,  0x3f, 0xfc, 0x0,  0x0,
    0x0, 0x0,  0xff, 0xff, 0x0,  0x0,  0x0,  0x1,  0xfc, 0x3f, 0x80, 0x0,
    0x0, 0x3,  0xe0, 0x7,  0xc0, 0x0,  0x0,  0x7,  0x80, 0x1,  0xe0, 0x0,
    0x0, 0xff, 0x0,  0x0,  0xff, 0x0,  0x3,  0xfe, 0x0,  0x0,  0x7f, 0xc0,
    0x7, 0xe2, 0x0,  0x0,  0x4f, 0xe0, 0x7,  0x80, 0x0,  0x0,  0x1,  0xe0,
    0xf, 0x0,  0x0,  0x0,  0x0,  0xf0, 0x1e, 0x0,  0x0,  0x0,  0x0,  0x70,
    0xe, 0x0,  0x0,  0x0,  0x0,  0x78, 0xc,  0x0,  0x0,  0x0,  0x0,  0x30,
    0xc, 0x0,  0x0,  0x0,  0x0,  0x30, 0xc,  0x0,  0x0,  0x0,  0x0,  0x30,
    0xe, 0x0,  0x0,  0x0,  0x0,  0x70, 0x1e, 0x0,  0x0,  0x0,  0x0,  0x70,
    0xf, 0x0,  0x0,  0x0,  0x0,  0xf0, 0x7,  0x80, 0x0,  0x0,  0x1,  0xe0,
    0x7, 0xe3, 0x80, 0x1,  0xc7, 0xe0, 0x3,  0xff, 0xc0, 0x3,  0xff, 0xc0,
    0x0, 0xff, 0xf0, 0xf,  0xff, 0x0,  0x0,  0x20, 0xfc, 0x3f, 0x4,  0x0,
    0x0, 0x0,  0x3f, 0xfc, 0x0,  0x0,  0x0,  0x0,  0xf,  0xf0, 0x0,  0x0,
    0x0, 0x4,  0x0,  0x0,  0x0,  0x0,  0x0,  0x4,  0x0,  0x0,  0x0,  0x0,
    0x0, 0xe,  0x0,  0x0,  0x0,  0x0,  0x0,  0xe,  0x0,  0x0,  0x0,  0x0,
    0x0, 0x1f, 0x0,  0x0,  0x0,  0x0,  0x0,  0x1f, 0x0,  0x0,  0x20, 0x0,
    0x0, 0x1f, 0x0,  0x0,  0x20, 0x0,  0x0,  0xe,  0x0,  0x0,  0x70, 0x0,
    0x0, 0x0,  0x1,  0x80, 0x70, 0x0,  0x0,  0x0,  0x1,  0x80, 0xf8, 0x0,
    0x0, 0x0,  0x3,  0xc0, 0xf8, 0x0,  0x0,  0x0,  0x3,  0xc0, 0xf8, 0x0,
    0x0, 0x0,  0x3,  0xc0, 0x70, 0x0,  0x0,  0x0,  0x3,  0xc0, 0x0,  0x0,
    0x0, 0x0,  0x3,  0xc0, 0x0,  0x0,  0x0,  0x0,  0x1,  0x80, 0x0,  0x0,
    0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,

};

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("DHTxx test!");

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

  // dht begin
  dht.begin();

  // set display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void showTemp(float temp, float hud) {
  // display.drawBitmap(0, 5, img, 48, 50, 1);
  display.setTextSize(1);
  display.setCursor(3, 0);
  display.println("Temperature/Humidity");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(60, 10);
  display.print(temp);
  display.println("°C");
  display.setCursor(60, 20);
  display.print(hud);
  display.println("%");
  display.display();
  display.clearDisplay();
}
void loop() {

  // delay(1000); // Wait a few seconds between measurements.
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // Read temperature as Celsius
  int sensorValue = analogRead(A0); // read the input on analog pin 0
  float voltage = sensorValue * (5.0 / 1023.0);
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Voltage: ");
  Serial.println(voltage); 
  Serial.println("°C");
  Serial.print("Humidity: "); // show in serial monitor
  Serial.print(h);
  Serial.print(" %, ");
  Serial.print("Temperature: "); // show in serial monitor
  Serial.print(t);
  Serial.println("°C");
  showTemp(t, h); // show temp

  client.loop();
  delay(1000);
}

// Read temperature
String readTemp() { return String(dht.readTemperature()); }

// MQTT callback function
void callback(char *topic, byte *payload, unsigned int length) {

  // Convert topic to string to make it easier to work with
  String topicStr = topic;
  // Get stuff working
  String cmd = String((char *)payload).substring(0, length);
  String temp = "S;3;";
  temp += readTemp();
  const char *msg = temp.c_str();
  if (cmd.equals("L:T")) {
    client.publish("bellax/ack", msg);
  }
}

void reconnect() {

  // attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {

    // loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  // make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {

      // Generate client name based on MAC address and last 8 bits of
      // microsecond counter
      String clientName;
      clientName += "BellaX-Hardware";

      // if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char *)clientName.c_str(), mqtt_username,
                         mqtt_password)) {
        // Serial.print("\tMTQQ Connected");
        client.subscribe(subTopic);
      }
    }
  }
}
