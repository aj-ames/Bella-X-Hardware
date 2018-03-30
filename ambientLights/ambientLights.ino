
#include<Adafruit_NeoPixel.h>
#define rgb 12

#define LDR A0

int value = 0;

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

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pixels.begin();
  goBlack();
  delay(10);
  
}

void loop() {
  value = analogRead(LDR);
  if(value < 100)
    whiteShow(255);
  else if(value > 850)
    whiteShow(0);
  else {
    value = 1024 - value;
    value /= 4;
    whiteShow(value);
  }
  delay(10);
}
