#include "LEDMatrix.h"
#include "TextRenderer.h"  // Include the TextRenderer header

void setup()
{
    ledSetup(); // Initialize the LED pin
}

void loop()
{
    // // Display a rainbow effect
    // rainbowCycle(100);

    // // Show red, green, blue, then clear
    // showColor(255, 0, 0); // Red
    // delay(1000);
    // showColor(0, 255, 0); // Green
    // delay(1000);
    // showColor(0, 0, 255); // Blue
    // delay(1000);
    // clearMatrix();
    // delay(1000);

    // // Display "Hello World"
    // renderText("HelloWrd");
    // delay(3000);  // Keep the text on screen for 3 seconds
    // clearMatrix(); // Clear the matrix
    // delay(1000);

    showColor(255, 255, 0);
}
