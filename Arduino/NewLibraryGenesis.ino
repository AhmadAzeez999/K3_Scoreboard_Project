/*
 * Optimized Neopixel driver for a 15x60 LED matrix.
 * Adapted to match original setup provided.
 */

// Matrix Configuration
#define MATRIX_WIDTH 60     // Width of the LED matrix
#define MATRIX_HEIGHT 15    // Height of the LED matrix
#define PIXELS (MATRIX_WIDTH * MATRIX_HEIGHT) // Total number of LEDs

// Pin Configuration
#define PIXEL_PORT PORTC    // Port connected to the LED strip data line
#define PIXEL_DDR  DDRC     // Data Direction Register for the port
#define PIXEL_BIT  2        // Bit number of the data pin (e.g., Digital Pin 10)

// Timing Parameters (for WS2812B)
#define T1H  814            // Width of a 1 bit in ns
#define T1L  438            // Width of a 0 bit in ns
#define T0H  312            // Width of a 0 bit in ns
#define T0L  936            // Width of a 0 bit in ns
#define RES 250000          // Reset time in ns

// Macros for nanoseconds-to-cycles conversion
#define NS_PER_SEC (1000000000L)
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

// Inline Function to Send a Bit
inline void sendBit(bool bitVal) {
    if (bitVal) {
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
    } else {
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

// Inline Function to Send a Byte
inline void sendByte(uint8_t byte) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        sendBit(byte & 0x80); // Send the highest bit first
        byte <<= 1;          // Shift left to process the next bit
    }
}

// LED Setup Function
void ledSetup() {
    bitSet(PIXEL_DDR, PIXEL_BIT); // Set the data pin as output
}

// Send Pixel Color (GRB Format)
inline void sendPixel(uint8_t r, uint8_t g, uint8_t b) {
    sendByte(g); // Send Green byte
    sendByte(r); // Send Red byte
    sendByte(b); // Send Blue byte
}

// Show Function: Latch Data to LEDs
void show() {
    _delay_us((RES / 1000UL) + 1); // Ensure a reset pulse
}

// Clear the Matrix
void clearMatrix() {
    for (int i = 0; i < PIXELS; i++) {
        sendPixel(0, 0, 0); // Set all pixels to black
    }
    show(); // Update LEDs
}

// Show Color on Entire Matrix
void showColor(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < PIXELS; i++) {
        sendPixel(r, g, b);
    }
    show(); // Update LEDs
}

// Render a Simple Animation (Rainbow Effect)
void rainbowCycle(uint16_t cycles) {
    for (uint16_t frame = 0; frame < cycles; frame++) {
        for (int i = 0; i < PIXELS; i++) {
            uint8_t hue = (i * 256 / PIXELS + frame) & 0xFF;
            uint8_t r = (hue < 85) ? hue * 3 : (hue < 170) ? (255 - hue) * 3 : 0;
            uint8_t g = (hue < 85) ? (255 - hue) * 3 : (hue < 170) ? 0 : (hue - 170) * 3;
            uint8_t b = (hue < 85) ? 0 : (hue < 170) ? (hue - 85) * 3 : (255 - hue) * 3;
            sendPixel(r, g, b);
        }
        show();
        delay(50); // Adjust delay for animation speed
    }
}

// Setup Function
void setup() {
    ledSetup(); // Initialize the LED pin
}

// Main Loop
void loop() {
    // Display a rainbow effect
    rainbowCycle(100); 

    // Show red, green, blue, then clear
    showColor(255, 0, 0); // Red
    delay(1000);
    showColor(0, 255, 0); // Green
    delay(1000);
    showColor(0, 0, 255); // Blue
    delay(1000);
    clearMatrix();
    delay(1000);
}
