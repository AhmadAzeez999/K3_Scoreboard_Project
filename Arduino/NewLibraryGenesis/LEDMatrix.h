#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h> // for uint8_t, uint16_t types

// Matrix Configuration
#define MATRIX_WIDTH 60
#define MATRIX_HEIGHT 15
#define PIXELS (MATRIX_WIDTH * MATRIX_HEIGHT)

// Pin Configuration
#define PIXEL_PORT PORTC
#define PIXEL_DDR  DDRC
#define PIXEL_BIT  2

// Function Declarations
void ledSetup();
void sendPixel(uint8_t r, uint8_t g, uint8_t b);
void clearMatrix();
void showColor(uint8_t r, uint8_t g, uint8_t b);
void rainbowCycle(uint16_t cycles);

#endif
