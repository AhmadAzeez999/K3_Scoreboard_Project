#include <Adafruit_NeoPixel.h>
#include "Base.h"
#include "Display.h"

void setup() 
{
  base::initializeAllStrips();
  // display::displayA(); // For displaying an A (make sure nothing is running in the loop() to test this)
}

void loop()
{  
  base::fillBoard(); // To fill the entire board
  delay(3000); // To wait for 3 seconds
  base::clearBoard(); // To clear the entire board
  delay(1000); // To wait for 1 second
}
