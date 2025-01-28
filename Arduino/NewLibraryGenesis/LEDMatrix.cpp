// LEDMatrix.cpp
#include "LEDMatrix.h"
#include <avr/io.h>   // For AVR-specific register access
#include <util/delay.h> // For _delay_us()

// Timing Parameters for WS2812B
#define T1H  814
#define T1L  438
#define T0H  312
#define T0L  936
#define RES 250000

#define NS_PER_SEC (1000000000L)
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

// Inline function to send a bit
inline void sendBit(bool bitVal)
{
    if (bitVal)
    {
        asm volatile (
            "sbi %[port], %[bit] \n\t"
            ".rept %[onCycles] \n\t"
            "nop \n\t"
            ".endr \n\t"
            "cbi %[port], %[bit] \n\t"
            ".rept %[offCycles] \n\t"
            "nop \n\t"
            ".endr \n\t"
            ::
            [port] "I" (_SFR_IO_ADDR(PIXEL_PORT)),
            [bit] "I" (PIXEL_BIT),
            [onCycles] "I" (NS_TO_CYCLES(T1H) - 2),
            [offCycles] "I" (NS_TO_CYCLES(T1L) - 2)
        );
    }
    else
    {
        asm volatile (
            "sbi %[port], %[bit] \n\t"
            ".rept %[onCycles] \n\t"
            "nop \n\t"
            ".endr \n\t"
            "cbi %[port], %[bit] \n\t"
            ".rept %[offCycles] \n\t"
            "nop \n\t"
            ".endr \n\t"
            ::
            [port] "I" (_SFR_IO_ADDR(PIXEL_PORT)),
            [bit] "I" (PIXEL_BIT),
            [onCycles] "I" (NS_TO_CYCLES(T0H) - 2),
            [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
        );
    }
}

// Inline function to send a byte
inline void sendByte(uint8_t byte)
{
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        sendBit(byte & 0x80);
        byte <<= 1;
    }
}

// LED Setup function
void ledSetup()
{
    PIXEL_DDR |= (1 << PIXEL_BIT); // Set the bit corresponding to PIXEL_BIT
}

// Send a pixel color (GRB format)
void sendPixel(uint8_t r, uint8_t g, uint8_t b)
{
    sendByte(g); // Send Green byte
    sendByte(r); // Send Red byte
    sendByte(b); // Send Blue byte
}

// Latch data to LEDs
void show()
{
    _delay_us((RES / 1000UL) + 1);
}

// Clear the matrix
void clearMatrix()
{
    for (int i = 0; i < PIXELS; i++)
    {
        sendPixel(0, 0, 0); // Set all pixels to black
    }
    show();
}

// Show color on the entire matrix
void showColor(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < PIXELS; i++)
    {
        sendPixel(r, g, b);
    }
    show();
}

// Rainbow effect (animation)
void rainbowCycle(uint16_t cycles)
{
    for (uint16_t frame = 0; frame < cycles; frame++)
    {
        for (int i = 0; i < PIXELS; i++)
        {
            uint8_t hue = (i * 256 / PIXELS + frame) & 0xFF;
            uint8_t r = (hue < 85) ? hue * 3 : (hue < 170) ? (255 - hue) * 3 : 0;
            uint8_t g = (hue < 85) ? (255 - hue) * 3 : (hue < 170) ? 0 : (hue - 170) * 3;
            uint8_t b = (hue < 85) ? 0 : (hue < 170) ? (hue - 85) * 3 : (255 - hue) * 3;
            sendPixel(r, g, b);
        }
        show();
        _delay_ms(50); // Adjust delay for animation speed
    }
}
