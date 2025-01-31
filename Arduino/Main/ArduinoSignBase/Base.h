#include <Adafruit_NeoPixel.h>

#define PIN 30
#define NUMPIXELS 120
#define NUM_STRIPS 7  // Number of strips

extern Adafruit_NeoPixel strips[NUM_STRIPS];
extern Adafruit_NeoPixel middleStrip;

namespace base
{
  // Functions
  void initializeAllStrips();
  void fillBoard();
  void clearBoard();
  void updateAllStrips();
}