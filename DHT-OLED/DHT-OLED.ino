#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESP8266WiFi.h"
#include "DHT.h"

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);
#define DHTPIN 14   // Connected to Pin D5 in NodeMCU

#define LDR A0

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE, 15);

float humidity = 0.0, temperature = 0.0;
int light = 0;
 
void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
  pinMode(LDR, INPUT);

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
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();   // Read temperature as Celsius
 
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void readLDR() {
  light = analogRead(LDR);
  Serial.println(light);
}
void loop() {
  int startTime = millis();
  while(millis() - startTime < 1000) {
    readLDR();
    delay(100);
    showValues(temperature, humidity, light);  
  }
  readDHT();
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
