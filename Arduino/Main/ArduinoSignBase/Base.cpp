#include <Adafruit_NeoPixel.h>
#include "Base.h"

#define PIN 30
#define NUMPIXELS 120
#define NUM_STRIPS 7  // Number of strips

// Declare an array of Adafruit_NeoPixel objects
Adafruit_NeoPixel strips[NUM_STRIPS] = 
{
  Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 1, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 3, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 6, NEO_GRB + NEO_KHZ800)
};

Adafruit_NeoPixel middleStrip = Adafruit_NeoPixel(NUMPIXELS-60, PIN + 7, NEO_GRB + NEO_KHZ800);

namespace base
{
  void initializeAllStrips()
  {
    // Initialize all strips
    for (int i = 0; i < NUM_STRIPS; i++) 
    {
      strips[i].begin();
      strips[i].setBrightness(50); // This could be moved to it's own function in the future
    }
    
    middleStrip.begin();
  }

  void fillBoard()
  {
    // Loop to light up all strips
    for (int y = 0; y < NUMPIXELS - 60; y++) 
    {
      // Set red color for each strip at the same position
      for (int i = 0; i < NUM_STRIPS; i++) 
      {
        strips[i].setPixelColor(y, 255, 0, 0);  // Red
      }
      middleStrip.setPixelColor(y, 255, 0, 0);
    }
    

    // Loop to light up the second half of the LEDs
    for (int y = 60; y < NUMPIXELS; y++) 
    {
      // Set red color for each strip at the same position
      for (int i = 0; i < NUM_STRIPS; i++) 
      {
        strips[i].setPixelColor(y, 255, 0, 0);  // Red
      }
    }

    updateAllStrips();
  }

  void clearBoard()
  {
      // Loop to clear all strips
    for (int y = 0; y < NUMPIXELS - 60; y++) 
    {
      for (int i = 0; i < NUM_STRIPS; i++) 
      {
        strips[i].clear();
      }

      middleStrip.clear();
    }
    

    // Loop to clear the second half of the LEDs
    for (int y = 60; y < NUMPIXELS; y++) 
    {
      for (int i = 0; i < NUM_STRIPS; i++) 
      {
        strips[i].clear();
      }
    }

    updateAllStrips();
  }

  // This function is for calling the ".show()" method for each strip
  void updateAllStrips()
  {
    // Loop to update all strips
    for (int y = 0; y < NUMPIXELS - 60; y++) 
    {
      for (int i = 0; i < NUM_STRIPS; i++) 
      {
        strips[i].show();  // Update the strip
      }
      middleStrip.show();
    }
    

    // Loop to update the second half of the LEDs
    for (int y = 60; y < NUMPIXELS; y++) 
    {
      for (int i = 0; i < NUM_STRIPS; i++) 
      {
        strips[i].show();  // Update the strip
      }
    }
  }
}