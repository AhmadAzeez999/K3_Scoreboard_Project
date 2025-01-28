// #include "TextRenderer.h"

// // Font data: Each byte represents a column (5x7 font, 7 rows)
// const uint8_t font[95][5] = {
//     // Space (ASCII 32)
//     {0x00, 0x00, 0x00, 0x00, 0x00},
//     // H (ASCII 72)
//     {0x1F, 0x10, 0x10, 0x10, 0x1F},
//     // E (ASCII 69)
//     {0x1F, 0x15, 0x15, 0x15, 0x11},
//     // L (ASCII 76)
//     {0x1F, 0x01, 0x01, 0x01, 0x01},
//     // O (ASCII 79)
//     {0x0E, 0x11, 0x11, 0x11, 0x0E},
//     // W (ASCII 87)
//     {0x1F, 0x08, 0x04, 0x08, 0x1F},
//     // R (ASCII 82)
//     {0x1F, 0x15, 0x15, 0x15, 0x1A},
//     // D (ASCII 68)
//     {0x1F, 0x11, 0x11, 0x11, 0x0E},
//     // ... (other characters)
// };

// // Render the text "Hello World" on the LED matrix
// void renderText(const char* text)
// {
//     int x = 0; // Starting x position on the matrix
//     int y = 0; // Starting y position on the matrix

//     for (int i = 0; text[i] != '\0'; i++)
//     {
//         renderChar(text[i], x, y);
//         x += 6; // Move to the next character (5 pixels for the char + 1 pixel for spacing)
        
//         if (x >= MATRIX_WIDTH)
//         {
//             x = 0;
//             y += 8; // Move to the next line
//         }
//     }
// }

// // Render a single character on the matrix at position (x, y)
// void renderChar(char c, int x, int y)
// {
//     if (c < 32 || c > 126) return; // Invalid character, outside the printable ASCII range
    
//     int charIndex = c - 32; // Convert ASCII to font index (ASCII 32 maps to 0)
    
//     for (int col = 0; col < 5; col++)
//     {
//         uint8_t columnData = font[charIndex][col];
        
//         for (int row = 0; row < 7; row++)
//         {
//             if (columnData & (1 << row)) // Check if the pixel should be on
//             {
//                 if (x + col < MATRIX_WIDTH && y + row < MATRIX_HEIGHT)
//                 {
//                     // Calculate the pixel index in the matrix
//                     int pixelIndex = (y + row) * MATRIX_WIDTH + (x + col);
//                     // Send the pixel data to the correct position
//                     sendPixel(255, 255, 255); // Display pixel (white color)
//                 }
//             }
//         }
//     }
// }