#include <Adafruit_NeoPixel.h>
#include "Display.h"

namespace display
{
  void displayPixel(int row, int col)
  {
    strips[row].begin();

    strips[row].setPixelColor(col, 255, 0, 0);
    strips[row].show();
  }

  void displayA() // This is a temporary function and for testing only
  {
    display::displayPixel(0, 1);
    display::displayPixel(0, 2);
    display::displayPixel(0, 3);
    display::displayPixel(1, 0);
    display::displayPixel(1, 4);
    display::displayPixel(2, 0);
    display::displayPixel(2, 4);
    display::displayPixel(3, 0);
    display::displayPixel(3, 1);
    display::displayPixel(3, 2);
    display::displayPixel(3, 3);
    display::displayPixel(3, 4);
    display::displayPixel(4, 0);
    display::displayPixel(4, 4);
    display::displayPixel(5, 0);
    display::displayPixel(5, 4);
  }
}