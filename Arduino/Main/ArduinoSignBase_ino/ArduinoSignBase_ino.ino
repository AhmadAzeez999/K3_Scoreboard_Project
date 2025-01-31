#include <Adafruit_NeoPixel.h>

#define PIN 30
#define NUMPIXELS 120
#define NUM_STRIPS 7  // Number of strips

// Declare an array of Adafruit_NeoPixel objects
Adafruit_NeoPixel strips[NUM_STRIPS] = {
  Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 1, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 3, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 6, NEO_GRB + NEO_KHZ800)
};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS-60, PIN + 7, NEO_GRB + NEO_KHZ800);


void setup() 
{
  // Initialize all strips
  
  for (int i = 0; i < NUM_STRIPS; i++) 
  {
    strips[i].begin();
    strips[i].setBrightness(50);
  }
  
  strip.begin();
  //strips[1].begin();
  
  

 
}

void loop() 
{  
  //strips[1].setPixelColor(6, 255, 0, 0);
  //strips[1].show();
  // Loop to light up all strips
  
  
  for (int y = 0; y < NUMPIXELS - 60; y++) 
  {
    // Set red color for each strip at the same position
    for (int i = 0; i < NUM_STRIPS; i++) 
    {
      strips[i].setPixelColor(y, 255, 0, 0);  // Red
      strips[i].show();  // Update the strip
    }
    strip.setPixelColor(y, 255, 0, 0);
    strip.show();
  }
  

  // Loop to light up the second half of the LEDs
  for (int y = 60; y < NUMPIXELS; y++) 
  {
    // Set red color for each strip at the same position
    for (int i = 0; i < NUM_STRIPS; i++) 
    {
      strips[i].setPixelColor(y, 255, 0, 0);  // Red
      strips[i].show();  // Update the strip
    }
  }
  

  
  
}
