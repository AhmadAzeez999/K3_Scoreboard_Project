#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "LEDMatrix.h" // Assuming LEDMatrix functions like sendPixel, clearMatrix, showColor are already declared

// Font data (5x7 font, each character is 5 columns wide and 7 rows high)
extern const uint8_t font[95][5]; // Array for the font (ASCII 32-126)

// Function declarations
void renderText(const char* text);
void renderChar(char c, int x, int y);

#endif
