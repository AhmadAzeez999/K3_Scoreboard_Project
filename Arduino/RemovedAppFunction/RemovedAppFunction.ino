#include <IRremote.h> //include the library
bool rOO = false; //is the remote on or off
bool rEM = false; //are you entering a manual countdown time "rEM"
bool vP = false; //should we be sending voltage out
uint8_t rEC = 0; //remote enter count...used to determine
uint8_t vPin = 17;
uint8_t vPin2 = 16;


#include "RTClib.h"
RTC_DS3231 rtc;
DateTime rtcStart;
DateTime rtcEnd;

bool flStarted = false;

bool cSC = false;  //capture Scroll Char
bool sT = false; //scrolling text if text is scrolling....keep scrolling do not scroll and pause
bool cdOnly = true; //countdown only mode.  displays stopped time.

uint8_t clrPat = 0; 
//Brightness Array
//100%	FF
//95%	F2
//90%	E6
//85%	D9
//80%	CC
//75%	BF
//70%	B3
//65%	A6
//60%	99
//55%	8C
//50%	80
//45%	73
//40%	66
//35%	59
//30%	4D
//25%	40
//20%	33
//15%	26
//10%	1A
//5%	0D
//0%	00
uint8_t br[10] = {0x00, 0x1A, 0x33, 0x4D, 0x66, 0x73, 0x80, 0x99, 0xB3, 0xCC};
//TOP COLOUR
uint8_t brLvl = 1;
uint8_t rVT = br[0];
uint8_t gVT = br[brLvl];
uint8_t bVT = br[0];
//BOTTOM COLOUR
uint8_t rVB = br[0];
uint8_t gVB = br[brLvl];
uint8_t bVB = br[brLvl];
//RESULT section COLOUR
uint8_t rVR = br[brLvl];
uint8_t gVR = br[0];
uint8_t bVR = br[0];

//TIMINING SPECIFIC variables
uint8_t tS = 0;  //timer Seconds
uint8_t tSC = 0;//timer Seconds Current
uint8_t tM = 0;  //timer minuutes
char cdTS[6]; //count down time String
bool tSed = false; //timer started  - used to control what to show based on if the clock is running or not
unsigned long clT = millis(); //cleared time
unsigned long cT = millis(); // current time
uint8_t cdTimeIn[4];
unsigned long cdTime;
unsigned long cdDiff;
unsigned long timeDiff;
bool cdR = false; //countdown running
bool dCD = false; //display countdown
bool externalCount = false;  //countdown from fieldlynx

//TEXT section
uint8_t cC = 0;  //char count controls the number of chars that are in the input set
int B;  //byte recieved.  stores the incoming byte
uint8_t subR = 1;  //sub routine used to determine what routine to display, timer, scroll and pause etc.
int sDel = 1000; // show delay //number of miliseconds to scroll across 20 chars
const uint8_t maxChar = 200;  //max number of characters that scroll accross the display
const uint8_t numSmChar = 21;
char msg20[numSmChar];
char msg20Copy[numSmChar];
char dataString[maxChar];//the main string of data
char dataStringCopy[numSmChar]; //can be used to display on the screen before scrolling off
char ipString[numSmChar]; //can be used to display on the screen before scrolling off
char cVar = ' '; //control varaiable.  leading variable passed to the display to control feature options

bool isVertical = false;
char *strings[7];
char *ptr;
char *nPtr;
char *name[2];



const uint8_t numBgChar = 6;
char msg6[numBgChar];
//char msg6Copy[numBgChar];

const uint8_t numHjLg = 7;
char msgHjLg[numHjLg];


#define PIXELS 120  //number of pixels in one strip of lights

#define PIXEL_PORTC  PORTC// Port of the pin the pixels are connected to  //sets what the pins are
#define PIXEL_DDRC  DDRC   // Port of the pin the pixels are connected to  //sets what the pins do

static const uint8_t fullBits =  0b11111111;
//static const uint8_t onBits =  0b11111100; //bits on the first set to show

#define T1H  814    // Width of a 1 bit in ns - 13 cycles
#define T1L  438    // Width of a 1 bit in ns -  7 cycles

#define T0H  312    // Width of a 0 bit in ns -  5 cycles
#define T0L  936    // Width of a 0 bit in ns - 15 cycles 

// Phase #1 - Always 1  - 5 cycles
// Phase #2 - Data part - 8 cycles
// Phase #3 - Always 0  - 7 cycles


#define RES 10000000   // Width of the low gap between bits to cause a frame to latch
#define NS_PER_SEC (1000000000L)
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )


static inline void sendBitx8(  const uint8_t row, const uint8_t colorbyte, const uint8_t onBits) {

  asm volatile (

    "L_%=: \n\r"
    "out %[port], %[onBits] \n\t"
    // (1 cycles) - send either T0H or the first part of T1H.
    // Onbits is a mask of which bits have strings attached.
    // Next determine if we are going to be sending 1s or 0s based on the current bit in the color....

    "mov r0, %[bitwalker] \n\t"               // (1 cycles)
    "and r0, %[colorbyte] \n\t"               // (1 cycles)  - is the current bit in the color byte set?
    "breq OFF_%= \n\t"                        // (1 cycles) - bit in color is 0, then send full zero row
    //(takes 2 cycles if branch taken, count the extra 1 on the target line)

    // If we get here, then we want to send a 1 for every row that has an ON dot...
    "nop \n\t  "
    "out %[port], %[row]   \n\t"                  // (1 cycles) - set the output bits to [row]
    //This is phase for T0H-T1H.
    // ==========
    // (5 cycles) - T0H (Phase #1)


    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t "                                   // (1 cycles)

    "out %[port], __zero_reg__ \n\t"              // (1 cycles) - set the output bits to 0x00
    //based on the bit in colorbyte. This is phase for T0H-T1H
    // ==========
    // (8 cycles) - Phase #2

    "ror %[bitwalker] \n\t"                      // (1 cycles) - get ready for next pass.
    //On last pass, the bit will end up in C flag

    "brcs DONE_%= \n\t"                          // (1 cycles) Exit if carry bit is set as a
    //result of us walking all 8 bits. We assume that the process around us will tak long enough
    //to cover the phase 3 delay

    "nop \n\t \n\t "                             // (1 cycles) - When added to the 5 cycles in S:,
    //we gte the 7 cycles of T1L

    "jmp L_%= \n\t"                              // (3 cycles)
    // (1 cycles) - The OUT on the next pass of the loop
    // ==========
    // (7 cycles) - T1L


    "OFF_%=: \n\r"                                // (1 cycles)    Note that we land here becuase of breq,
    //which takes takes 2 cycles

    "out %[port], __zero_reg__ \n\t"              // (1 cycles) - set the output bits to 0x00 based on the
    //bit in colorbyte. This is phase for T0H-T1H
    // ==========
    // (5 cycles) - T0H

    "ror %[bitwalker] \n\t"                      // (1 cycles) - get ready for next pass.
    //On last pass, the bit will end up in C flag

    "brcs DONE_%= \n\t"             // (1 cycles) Exit if carry bit is set as a
    //result of us walking all 8 bits. We assume that the
    //process around us will take long enough to cover the phase 3 delay

    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t nop \n\t "                          // (2 cycles)
    "nop \n\t "                                   // (1 cycles)

    "jmp L_%= \n\t"                               // (3 cycles)
    // (1 cycles) - The OUT on the next pass of the loop
    // ==========
    //(15 cycles) - T0L


    "DONE_%=: \n\t"

    // Don't need an explicit delay here since the overhead that follows will always be long enough

    ::
    [port]    "I" (_SFR_IO_ADDR(PIXEL_PORTC)),
    [row]   "d" (row),
    [onBits]   "d" (onBits),
    [colorbyte]   "d" (colorbyte ),     // Phase 2 of the signal where the actual data bits show up.
    [bitwalker] "r" (0x80)    // register to hold a bit that we will walk down though the color byte

  );

  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the reset timeout
  //(which is A long time)

}

// Send 3 bytes of color data (R,G,B) for a signle pixel down all the connected stringsat the same time
// A 1 bit in "row" means send the color, a 0 bit means send black.
static inline void sendRowRGB( uint8_t row,  uint8_t r,  uint8_t g,  uint8_t b ) {
  sendBitx8( row, g, fullBits);      // WS2812 takes colors in GRB order
  sendBitx8( row, r, fullBits);      // WS2812 takes colors in GRB order
  sendBitx8( row, b, fullBits);      // WS2812 takes colors in GRB order
}

// how long to scroll across 20 chars
void show() {
  delay(sDel);
}

// how long to scroll across 20 chars
void setsDel(int ms) {
  sDel = ms;
}

// This nice 5x7 font from here...
// http://sunge.awardspace.com/glcd-sd/node4.html

// Font details:
// 1) Each char is fixed 5x7 pixels.
// 2) Each byte is one column.
// 3) Columns are left to right order, leftmost byte is leftmost column of pixels.
// 4) Each column is 8 bits high.
// 5) Bit #7 is top line of char, Bit #1 is bottom.
// 6) Bit #0 is always 0, becuase this pin is used as serial input and setting to
// 1 would enable the pull-up.

// defines ascii characters 0x20-0x7F (32-127)
// PROGMEM after variable name as per https://www.arduino.cc/en/Reference/PROGMEM

#define FONT_WIDTH 5
#define BIG_FONT_WIDTH 10
#define INTERCHAR_SPACE 1

//0x00, 0x00, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, // `
//0x70, 0x38, 0x1c, 0x3e, 0xf0, 0x3e, 0x1c, 0x38, 0x70, 0x00, // rose
const uint8_t Font10x14t_[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
  0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, // !
  0x00, 0x00, 0x70, 0x70, 0x00, 0x00, 0x70, 0x70, 0x00, 0x00, // "
  0x06, 0x06, 0x3F, 0x3F, 0x06, 0x06, 0x3F, 0x3F, 0x06, 0x06, // #
  0x0E, 0x1F, 0x31, 0x31, 0x7F, 0x7F, 0x31, 0x31, 0x10, 0x00, // $
  0x00, 0x00, 0x18, 0x18, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x00, // %
  0x00, 0x0C, 0x1F, 0x33, 0x20, 0x21, 0x33, 0x1E, 0x0C, 0x00, // &
  0x00, 0x00, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '
  0x00, 0x00, 0x1F, 0x3F, 0x60, 0x60, 0x60, 0x40, 0x00, 0x00, // (
  0x00, 0x00, 0x40, 0x60, 0x60, 0x60, 0x3F, 0x1F, 0x00, 0x00, // )
  0x00, 0x54, 0x38, 0x7C, 0x38, 0x54, 0x00, 0x00, 0x00, 0x00, // *
  0x00, 0x00, 0x01, 0x01, 0x07, 0x07, 0x01, 0x01, 0x00, 0x00, // +
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // comma
  0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, // -
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .
  0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0E, 0x1C, 0x38, // /
  0x1F, 0x3F, 0x60, 0x60, 0x60, 0x63, 0x66, 0x6C, 0x3F, 0x1F, // 0
  0x00, 0x00, 0x18, 0x38, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, // 1
  0x18, 0x38, 0x60, 0x60, 0x60, 0x60, 0x60, 0x61, 0x3F, 0x1E, // 2
  0x18, 0x38, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x3F, 0x1E, // 3
  0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x7F, 0x7F, 0x00, 0x00, // 4
  0x7F, 0x7F, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x61, 0x60, // 5
  0x07, 0x0F, 0x19, 0x31, 0x61, 0x61, 0x61, 0x61, 0x00, 0x00, // 6
  0x60, 0x60, 0x60, 0x60, 0x60, 0x61, 0x63, 0x66, 0x7C, 0x78, // 7
  0x1E, 0x3F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x3F, 0x1E, // 8
  0x1E, 0x3F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x3F, 0x1F, // 9
  0x00, 0x00, 0x00, 0x1C, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, // :
  0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00, // ;
  0x00, 0x00, 0x03, 0x07, 0x0E, 0x1C, 0x18, 0x00, 0x00, 0x00, // <
  0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, // =
  0x00, 0x00, 0x00, 0x18, 0x1C, 0x0E, 0x07, 0x03, 0x00, 0x00, // >
  0x00, 0x1C, 0x3C, 0x60, 0x60, 0x60, 0x61, 0x3F, 0x1F, 0x00, // ?
  0x07, 0x0F, 0x18, 0x19, 0x19, 0x19, 0x18, 0x0F, 0x07, 0x00, // @
  0x1F, 0x3F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x3F, 0x1F, // A
  0x7F, 0x7F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x3F, 0x1E, // B
  0x1F, 0x3F, 0x70, 0x60, 0x60, 0x60, 0x60, 0x70, 0x30, 0x00, // C
  0x7F, 0x7F, 0x60, 0x60, 0x60, 0x60, 0x60, 0x70, 0x3F, 0x1F, // D
  0x7F, 0x7F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x60, 0x00, // E
  0x7F, 0x7F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x60, 0x00, // F
  0x1F, 0x3F, 0x60, 0x60, 0x60, 0x60, 0x60, 0x70, 0x30, 0x10, // G
  0x7F, 0x7F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x7F, 0x7F, // H
  0x00, 0x60, 0x60, 0x60, 0x7F, 0x7F, 0x60, 0x60, 0x60, 0x00, // I
  0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7F, 0x7F, 0x60, 0x60, // J
  0x7F, 0x7F, 0x03, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0x60, 0x00, // K
  0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L
  0x3F, 0x7F, 0x60, 0x60, 0x3F, 0x3F, 0x60, 0x60, 0x7F, 0x3F, // M
  0x7F, 0x7F, 0x38, 0x1E, 0x07, 0x01, 0x00, 0x00, 0x7F, 0x7F, // N
  0x1F, 0x3F, 0x70, 0x60, 0x60, 0x60, 0x60, 0x70, 0x3F, 0x1F, // O
  0x7F, 0x7F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x7F, 0x1E, // P
  0x1F, 0x3F, 0x70, 0x60, 0x60, 0x60, 0x60, 0x70, 0x3F, 0x1F, // Q
  0x7F, 0x7F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x7F, 0x3F, // R
  0x1E, 0x3F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x60, 0x00, // S
  0x60, 0x60, 0x60, 0x60, 0x7F, 0x7F, 0x60, 0x60, 0x60, 0x60, // T
  0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, // U
  0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, // V
  0x7F, 0x7F, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x7F, 0x7F, // W
  0x60, 0x78, 0x1C, 0x0E, 0x07, 0x07, 0x0E, 0x1C, 0x78, 0x60, // X
  0x7C, 0x7E, 0x03, 0x01, 0x00, 0x00, 0x01, 0x03, 0x7E, 0x7C, // Y
  0x60, 0x60, 0x60, 0x60, 0x61, 0x63, 0x67, 0x6E, 0x7C, 0x78, // Z
  0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, // [
  0x38, 0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, // (bacslash)
  0x41, 0x41, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ]
  0x00, 0x00, 0x18, 0x30, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, // ^
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // _
  0x00, 0x00, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, // `
  0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, // a
  0x7F, 0x7F, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, // b
  0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, // c
  0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x7F, 0x7F, // d
  0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, //
  0x01, 0x01, 0x1F, 0x3F, 0x61, 0x61, 0x60, 0x38, 0x18, 0x00, // f
  0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, // g
  0x7F, 0x7F, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, // h
  0x00, 0x00, 0x00, 0x03, 0x1B, 0x1B, 0x00, 0x00, 0x00, 0x00, //
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x1B, 0x1B, 0x00, 0x00, // j
  0x00, 0x7F, 0x7F, 0x00, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, //
  0x00, 0x00, 0x60, 0x60, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, // l
  0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, // m
  0x03, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, //
  0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, // o
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, // p
  0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x03, 0x00, // q
  0x03, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, // r
  0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, // s
  0x00, 0x03, 0x03, 0x1F, 0x1F, 0x03, 0x03, 0x00, 0x00, 0x00, // t
  0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, // u
  0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, //
  0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, // w
  0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, // x
  0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, // y
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, // z
  0x00, 0x00, 0x1F, 0x3F, 0x60, 0x60, 0x60, 0x40, 0x00, 0x00, // {
  0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, // |
  0x00, 0x00, 0x40, 0x60, 0x60, 0x60, 0x3F, 0x1F, 0x00, 0x00, // }
  0x00, 0x01, 0x01, 0x01, 0x01, 0x0D, 0x07, 0x03, 0x01, 0x00, // ~
  0x08, 0x1C, 0x2A, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, //
};

//bottom of the 10 wide 14 high font
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // `
// 0x00, 0x80, 0xc0, 0x20, 0xfe, 0x08, 0x10, 0x60, 0x40, 0x00, // Rose
const uint8_t Font10x14b[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
  0x00, 0x00, 0x00, 0x00, 0xe6, 0xe6, 0x00, 0x00, 0x00, 0x00, // !
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // "
  0x60, 0x60, 0xfc, 0xfc, 0x60, 0x60, 0xfc, 0xfc, 0x60, 0x60, // #
  0x00, 0x08, 0x8c, 0x8c, 0xfe, 0xfe, 0x8c, 0x8c, 0xf8, 0x70, // $
  0x00, 0x30, 0x60, 0xc0, 0x80, 0x00, 0x30, 0x30, 0x00, 0x00, // %
  0x00, 0x1c, 0x36, 0xe2, 0xc2, 0xe2, 0x36, 0x1e, 0x34, 0x02, // &
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '
  0x00, 0x00, 0xf8, 0xfc, 0x06, 0x06, 0x06, 0x02, 0x00, 0x00, // (
  0x00, 0x00, 0x02, 0x06, 0x06, 0x06, 0xfc, 0xf8, 0x00, 0x00, // )
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // *
  0x00, 0x00, 0x80, 0x80, 0xe0, 0xe0, 0x80, 0x80, 0x00, 0x00, // +
  0x00, 0x00, 0x1a, 0x1a, 0x1a, 0x1c, 0x00, 0x00, 0x00, 0x00, // comma
  0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, // -
  0x00, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, // .
  0x1c, 0x38, 0x70, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, // /
  0xf8, 0xfc, 0x36, 0x66, 0xc6, 0x06, 0x06, 0x06, 0xfc, 0xf8, // 0
  0x00, 0x00, 0x06, 0x06, 0xfe, 0xfe, 0x06, 0x06, 0x00, 0x00, // 1
  0x06, 0x0e, 0x1e, 0x36, 0x66, 0xc6, 0x86, 0x06, 0x06, 0x06, // 2
  0x18, 0x1c, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xfc, 0x78, // 3
  0xe0, 0xe0, 0x60, 0x60, 0x60, 0x60, 0xfe, 0xfe, 0x60, 0x60, // 4
  0x18, 0x1c, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xfc, 0xf8, // 5
  0xf8, 0xfc, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xfc, 0x78, // 6
  0x00, 0x00, 0x7e, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, // 7
  0x78, 0xfc, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xfc, 0x78, // 8
  0x00, 0x00, 0x86, 0x86, 0x86, 0x86, 0x8c, 0x98, 0xf0, 0xe0, // 9
  0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, // :
  0x00, 0x00, 0x00, 0x68, 0x68, 0x68, 0x70, 0x00, 0x00, 0x00, // ;
  0x00, 0x00, 0xc0, 0xe0, 0x70, 0x38, 0x18, 0x00, 0x00, 0x00, // <
  0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, // =
  0x00, 0x00, 0x00, 0x18, 0x38, 0x70, 0xe0, 0xc0, 0x00, 0x00, // >
  0x00, 0x00, 0x00, 0x00, 0x76, 0xf6, 0xc0, 0x80, 0x00, 0x00, // ?
  0xf0, 0xf8, 0x04, 0xf6, 0x36, 0xf6, 0x36, 0xf2, 0xe0, 0x00, // @
  0xfe, 0xfe, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xfe, 0xfe, // A
  0xfe, 0xfe, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xfc, 0x78, // B
  0xf8, 0xfc, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x0e, 0x0c, 0x00, // C
  0xfe, 0xfe, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0e, 0xfc, 0xf8, // D
  0xfe, 0xfe, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x06, 0x00, // E
  0xfe, 0xfe, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, // F
  0xf8, 0xfc, 0x06, 0x06, 0xc6, 0xc6, 0xc6, 0xce, 0xfc, 0x78, // G
  0xfe, 0xfe, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xfe, 0xfe, // H
  0x00, 0x06, 0x06, 0x06, 0xfe, 0xfe, 0x06, 0x06, 0x06, 0x00, // I
  0x18, 0x1c, 0x06, 0x06, 0x06, 0x06, 0xfc, 0xf8, 0x00, 0x00, // J
  0xfe, 0xfe, 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x06, 0x00, // K
  0xfe, 0xfe, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, // L
  0xfe, 0xfe, 0x00, 0x00, 0xf8, 0xf8, 0x00, 0x00, 0xfe, 0xfe, // M
  0xfe, 0xfe, 0x00, 0x00, 0x80, 0xe0, 0x78, 0x1c, 0xfe, 0xfe, // N
  0xf8, 0xfc, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x0e, 0xfc, 0xf8, // O
  0xfe, 0xfe, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, // P
  0xf8, 0xfc, 0x0e, 0x06, 0x06, 0x06, 0x36, 0x1c, 0xfe, 0xf6, // Q
  0xfe, 0xfe, 0x80, 0x80, 0xe0, 0xf0, 0xb8, 0x9c, 0x8e, 0x06, // R
  0x00, 0x06, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xfc, 0x78, // S
  0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, // T
  0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xfc, 0xf8, // U
  0xe0, 0xf0, 0x18, 0x0c, 0x06, 0x06, 0x0c, 0x18, 0xf0, 0xe0, // V
  0xfc, 0xfe, 0x06, 0x06, 0xfc, 0xfc, 0x06, 0x06, 0xfe, 0xfc, // W
  0x06, 0x1e, 0x38, 0x70, 0xc0, 0xc0, 0x70, 0x38, 0x1e, 0x06, // X
  0x00, 0x00, 0x00, 0x80, 0xfe, 0xfe, 0x80, 0x00, 0x00, 0x00, // Y
  0x1e, 0x3e, 0x76, 0xe6, 0xc6, 0x86, 0x06, 0x06, 0x06, 0x06, // Z
  0x00, 0x00, 0xfe, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, // [
  0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x38, 0x1c, // (backslash)
  0x82, 0x82, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ]
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ^
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, // _
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // `
  0x1c, 0x3e, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0xfe, 0xfc, // a
  0xfe, 0xfe, 0x86, 0x06, 0x06, 0x06, 0x06, 0x06, 0xfc, 0xf8, // b
  0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x8e, 0x8c, // c
  0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xfe, 0xfe, // d
  0xf8, 0xfc, 0x26, 0x26, 0x26, 0x26, 0x26, 0xee, 0xcc, 0x00, // e
  0x80, 0x80, 0xfe, 0xfe, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, // f
  0xc0, 0xe0, 0x26, 0x26, 0x26, 0x26, 0x26, 0xfc, 0xf8, 0x00, // g
  0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, // h
  0x00, 0x00, 0x00, 0x06, 0xfe, 0xfe, 0x06, 0x00, 0x00, 0x00, // i
  0x00, 0x00, 0x0c, 0x0e, 0x06, 0x06, 0xfe, 0xfc, 0x00, 0x00, // j
  0x00, 0xfe, 0xfe, 0xe0, 0xf0, 0xb8, 0x1c, 0x0e, 0x06, 0x00, // k
  0x00, 0x00, 0x06, 0x06, 0xfe, 0xfe, 0x06, 0x06, 0x00, 0x00, // l
  0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, // m
  0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, // n
  0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x06, 0xfc, 0xf8, 0x00, // o
  0xfe, 0xfe, 0x20, 0x20, 0x20, 0x20, 0x20, 0xe0, 0xc0, 0x00, // p
  0xc0, 0xe0, 0x20, 0x20, 0x20, 0x20, 0xa0, 0xfe, 0xfe, 0x00, // q
  0xfe, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, // r
  0xc0, 0xe4, 0x26, 0x26, 0x26, 0x26, 0x26, 0x3c, 0x18, 0x00, // s
  0x00, 0x00, 0x00, 0xfe, 0xfe, 0x06, 0x06, 0x0e, 0x0c, 0x00, // t
  0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x04, 0xfe, 0xf6, 0x00, // u
  0xe0, 0xf0, 0x18, 0x0c, 0x06, 0x06, 0x0c, 0x18, 0xf0, 0xe0, // v
  0xfc, 0xfe, 0x06, 0x06, 0xfe, 0xfe, 0x06, 0x06, 0xfe, 0xfc, // w
  0x00, 0x06, 0x8c, 0xd8, 0x70, 0x70, 0xd8, 0x8c, 0x06, 0x00, // x
  0xc0, 0xe0, 0x60, 0x66, 0x66, 0x66, 0x66, 0xfe, 0xfc, 0x00, // y
  0x06, 0x0e, 0x1e, 0x36, 0x26, 0x66, 0xc6, 0x86, 0x06, 0x00, // z
  0x00, 0x00, 0xf8, 0xfc, 0x06, 0x06, 0x06, 0x02, 0x00, 0x00, // {
  0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, // |
  0x00, 0x00, 0x02, 0x06, 0x06, 0x06, 0xfc, 0xf8, 0x00, 0x00, // }
  0x00, 0x80, 0x80, 0x80, 0x80, 0xb0, 0xe0, 0xc0, 0x80, 0x00, // ~
  0x10, 0x38, 0x54, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, // 
};

//standard 5x7 font
const uint8_t Font5x7[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, //
  0x00, 0x00, 0xfa, 0x00, 0x00, // !
  0x00, 0xe0, 0x00, 0xe0, 0x00, // "
  0x28, 0xfe, 0x28, 0xfe, 0x28, // #
  0x24, 0x54, 0xfe, 0x54, 0x48, // $
  0xc4, 0xc8, 0x10, 0x26, 0x46, // %
  0x6c, 0x92, 0xaa, 0x44, 0x0a, // &
  0x00, 0xa0, 0xc0, 0x00, 0x00, // '
  0x00, 0x38, 0x44, 0x82, 0x00, // (
  0x00, 0x82, 0x44, 0x38, 0x00, // )
  0x10, 0x54, 0x38, 0x54, 0x10, // *
  0x10, 0x10, 0x7c, 0x10, 0x10, // +
  0x00, 0x0a, 0x0c, 0x00, 0x00, // comma
  0x10, 0x10, 0x10, 0x10, 0x10, // -
  0x00, 0x06, 0x06, 0x00, 0x00, // .
  0x04, 0x08, 0x10, 0x20, 0x40, // /
  0x7c, 0x8a, 0x92, 0xa2, 0x7c, // 0
  0x00, 0x42, 0xfe, 0x02, 0x00, // 1
  0x42, 0x86, 0x8a, 0x92, 0x62, // 2
  0x84, 0x82, 0xa2, 0xd2, 0x8c, // 3
  0x18, 0x28, 0x48, 0xfe, 0x08, // 4
  0xe4, 0xa2, 0xa2, 0xa2, 0x9c, // 5
  0x3c, 0x52, 0x92, 0x92, 0x0c, // 6
  0x80, 0x8e, 0x90, 0xa0, 0xc0, // 7
  0x6c, 0x92, 0x92, 0x92, 0x6c, // 8
  0x60, 0x92, 0x92, 0x94, 0x78, // 9
  0x00, 0x6c, 0x6c, 0x00, 0x00, // :
  0x00, 0x6a, 0x6c, 0x00, 0x00, // ;
  0x00, 0x10, 0x28, 0x44, 0x82, // <
  0x28, 0x28, 0x28, 0x28, 0x28, // =
  0x82, 0x44, 0x28, 0x10, 0x00, // >
  0x40, 0x80, 0x8a, 0x90, 0x60, // ?
  0x4c, 0x92, 0x9e, 0x82, 0x7c, // @
  0x7e, 0x88, 0x88, 0x88, 0x7e, // A
  0xfe, 0x92, 0x92, 0x92, 0x6c, // B
  0x7c, 0x82, 0x82, 0x82, 0x44, // C
  0xfe, 0x82, 0x82, 0x44, 0x38, // D
  0xfe, 0x92, 0x92, 0x92, 0x82, // E
  0xfe, 0x90, 0x90, 0x80, 0x80, // F
  0x7c, 0x82, 0x82, 0x8a, 0x4c, // G
  0xfe, 0x10, 0x10, 0x10, 0xfe, // H
  0x00, 0x82, 0xfe, 0x82, 0x00, // I
  0x04, 0x02, 0x82, 0xfc, 0x80, // J
  0xfe, 0x10, 0x28, 0x44, 0x82, // K     0xfe, 0x10, 0x28, 0x44, 0x82
  0xfe, 0x02, 0x02, 0x02, 0x02, // L
  0xfe, 0x40, 0x20, 0x40, 0xfe, // M
  0xfe, 0x20, 0x10, 0x08, 0xfe, // N
  0x7c, 0x82, 0x82, 0x82, 0x7c, // O
  0xfe, 0x90, 0x90, 0x90, 0x60, // P
  0x7c, 0x82, 0x8a, 0x84, 0x7a, // Q
  0xfe, 0x90, 0x98, 0x94, 0x62, // R
  0x62, 0x92, 0x92, 0x92, 0x8c, // S
  0x80, 0x80, 0xfe, 0x80, 0x80, // T
  0xfc, 0x02, 0x02, 0x02, 0xfc, // U
  0xf8, 0x04, 0x02, 0x04, 0xf8, // V
  0xfe, 0x04, 0x18, 0x04, 0xfe, // W
  0xc6, 0x28, 0x10, 0x28, 0xc6, // X
  0xc0, 0x20, 0x1e, 0x20, 0xc0, // Y
  0x86, 0x8a, 0x92, 0xa2, 0xc2, // Z
  0x00, 0x00, 0xfe, 0x82, 0x82, // [
  0x40, 0x20, 0x10, 0x08, 0x04, // (backslash)
  0x82, 0x82, 0xfe, 0x00, 0x00, // ]
  0x20, 0x40, 0x80, 0x40, 0x20, // ^
  0x02, 0x02, 0x02, 0x02, 0x02, // _
  0x00, 0x80, 0x40, 0x20, 0x00, // `
  0x04, 0x2a, 0x2a, 0x2a, 0x1e, // a
  0xfe, 0x12, 0x22, 0x22, 0x1c, // b
  0x1c, 0x22, 0x22, 0x22, 0x04, // c
  0x1c, 0x22, 0x22, 0x12, 0xfe, // d
  0x1c, 0x2a, 0x2a, 0x2a, 0x18, // e
  0x10, 0x7e, 0x90, 0x80, 0x40, // f
  0x10, 0x28, 0x2a, 0x2a, 0x3c, // g
  0xfe, 0x10, 0x20, 0x20, 0x1e, // h
  0x00, 0x22, 0xbe, 0x02, 0x00, // i
  0x04, 0x02, 0x22, 0xbc, 0x00, // j
  0x00, 0xfe, 0x08, 0x14, 0x22, // k
  0x00, 0x82, 0xfe, 0x02, 0x00, // l
  0x3e, 0x20, 0x18, 0x20, 0x1e, // m
  0x3e, 0x10, 0x20, 0x20, 0x1e, // n
  0x1c, 0x22, 0x22, 0x22, 0x1c, // o
  0x3e, 0x28, 0x28, 0x28, 0x10, // p
  0x10, 0x28, 0x28, 0x18, 0x3e, // q
  0x3e, 0x10, 0x20, 0x20, 0x10, // r
  0x12, 0x2a, 0x2a, 0x2a, 0x04, // s
  0x20, 0xfc, 0x22, 0x02, 0x04, // t
  0x3c, 0x02, 0x02, 0x04, 0x3e, // u
  0x38, 0x04, 0x02, 0x04, 0x38, // v
  0x3c, 0x02, 0x0c, 0x02, 0x3c, // w
  0x22, 0x14, 0x08, 0x14, 0x22, // x
  0x30, 0x0a, 0x0a, 0x0a, 0x3c, // y
  0x22, 0x26, 0x2a, 0x32, 0x22, // z
  0x00, 0x10, 0x6c, 0x82, 0x00, // {
  0x00, 0x00, 0xfe, 0x00, 0x00, // |
  0x00, 0x82, 0x6c, 0x10, 0x00, // }
  0x10, 0x10, 0x54, 0x38, 0x10, // ~
  0x10, 0x38, 0x54, 0x10, 0x10, // 
};

//clear LEDs
static inline void clear() {
  cli();
  for ( unsigned int i = 0; i < PIXELS; i++ ) {
    sendRowRGB( 0, 0, 0, 0 );
  }
  sei();
  show();
}

void setSR(uint8_t r) {
  subR = r;
}

//load default message
void loadDefault() {
  //load standard default message and don't scroll anymore
  setsDel(200);
  //scrollAndPauseTop(" Athletics", 10, ' ');
  displaySmallText(" Athletics  Canada  ", 30);

  if (tSed) {
    setSR(12);
  } else {
    setSR(0);
  }
  
  delay(1000);
}

//clear the dataString that was last entered
void clearDataString() {
  for (uint8_t i = 0; i < maxChar; i++) {
    dataString[i] = ' ';
  }
}

//copy data string only enough to static load the screen with the last next string
//kevinkevin
void copyDataString() {
  for (uint8_t i = 0; i < numSmChar; i++) {
    dataStringCopy[i] = dataString[i];
  }
  
  for (uint8_t i = 0; i < numSmChar; i++) {
    if (dataString[i]) {
      msg20Copy[i] = dataString[i];
    }
  }
}

//color update sets what was sent from the display to the values that are used
void colorUpdate() {
  if(clrPat == 0){
    gVT = br[brLvl];
    bVT = br[0];
    rVT = br[0];

    gVB = br[brLvl];
    bVB = br[brLvl];
    rVB = br[0];

    gVR = br[0];
    bVR = br[brLvl];
    rVR = br[brLvl];
  }
  else if(clrPat == 1){
    gVT = br[0];
    bVT = br[brLvl];
    rVT = br[brLvl];

    gVB = br[brLvl];
    bVB = br[0];
    rVB = br[0];

    gVR = br[brLvl];
    bVR = br[brLvl];
    rVR = br[0];
  }
  else if(clrPat == 2){
    gVT = br[brLvl];
    bVT = br[brLvl];
    rVT = br[0];

    gVB = br[0];
    bVB = br[brLvl];
    rVB = br[brLvl];

    gVR = br[brLvl];
    bVR = br[0];
    rVR = br[0];
  }
  else if(clrPat == 3){
    gVT = br[brLvl];
    bVT = br[0];
    rVT = br[0];

    gVB = br[brLvl];
    bVB = br[brLvl];
    rVB = br[0];

    gVR = br[brLvl];
    bVR = br[brLvl];
    rVR = br[0];
  }
}

void clrMes(){
  colorUpdate();
  if(clrPat == 0){
    displaySmallText("Green-----Blue-Purpl", 15);
  }
  else if(clrPat == 1){
    displaySmallText("Purple----Grn--Blue-", 15);
  }
  else if(clrPat == 2){
    displaySmallText("Blue------Prpl-Green", 15);
  }
  else if(clrPat == 3){
    displaySmallText("--Green------Blue---", 15);
  }
}

void patChgR(){
  if(clrPat == 3){
    clrPat = 0;
  }
  else{
    clrPat++;
  }
  clrMes();
}

void patChgL(){
  if(clrPat == 0){
    clrPat = 3;
  }
  else{
    clrPat--;
  }
  clrMes();
}

void brightUp() {
  if(brLvl == 9){
    displayBig1Special("Br.Max");
  }
  else{
    brLvl++;
    colorUpdate();
    displayBig1Special("Br.Up ");
  }

}

void brightDown() {
  if(brLvl == 1){
    displayBig1Special("Br.Min");
  }
  else{
    brLvl--;
    colorUpdate();
    displayBig1Special("Br.Dwn");
  }
}

// Send the pixels to form the specified char, not including interchar space
// skip is the number of pixels to skip at the begining to enable sub-char smooth scrolling

static inline void sendChar( uint8_t c,  uint8_t skip, uint8_t r,  uint8_t g,  uint8_t b ) {
  const uint8_t *charbase = Font5x7 + (( c - ' ') * 5 ) ;
  uint8_t col = 5;

  while (skip--) {
    charbase++;
    col--;
  }

  while (col--) {
    sendRowRGB( pgm_read_byte_near( charbase++ ), r, g, b );
  }

  col = 1;

  while (col--) {
    sendRowRGB( 0, r, g, b );       // Interchar space
  }
}

static inline void sendBigCharT( uint8_t c,  uint8_t skip) {
  const uint8_t *charbase = Font10x14t_ + (( c - ' ') * 10 ) ;
  uint8_t col = 10;

  while (skip--) {
    charbase++;
    col--;
  }

  while (col--) {
    sendRowRGB( pgm_read_byte_near( charbase++ ), rVT, gVT, bVT );
  }

  col = 1;

  while (col--) {
    sendRowRGB(0, rVT, gVT, bVT );       // Interchar space
  }

}

static inline void sendBigCharB( uint8_t c,  uint8_t skip) {
  const uint8_t *charbase = Font10x14b + (( c - ' ') * 10 ) ;
  uint8_t col = 10;

  while (skip--) {
    charbase++;
    col--;
  }

  while (col--) {
    sendRowRGB( pgm_read_byte_near( charbase++ ), rVT, gVT, bVT );
  }

  col = 1;

  while (col--) {
    sendRowRGB( 0, rVT, gVT, bVT );       // Interchar space
  }

}

static inline void sendStringBig( const char *s, uint8_t skip) {
  unsigned int l = PIXELS / (5 + 1);
  sendBigCharT(' ', skip); //always send blank so don't have half and half char

  uint8_t currentChar = 0;
  while ( *(++s) && l--) {
    //certain spot send blank, otherwise send char
    if (currentChar < 5) { //big font is double width so top is 9 wide
      if (currentChar == 4) {
        sendBigCharT(' ', 0);
        sendRowRGB( 0, rVT, gVT, bVT );
      }
      else {
        sendBigCharT(*s, 0);
        sendRowRGB( 0, rVT, gVT, bVT );
      }
    }
    //certain spot send blank, otherwise send char
    else {
      if (currentChar == 9) { //bottom char on dual wide
        sendBigCharB(' ', 0);
        sendRowRGB( 0, rVT, gVT, bVT );
      }
      else {
        sendBigCharB( *s, 0);
        sendRowRGB( 0, rVT, gVT, bVT );
      }
    }
    currentChar++;
  }
  show();
}

static inline void scrollBig( char *m, uint8_t scrollLength) {
  //create blank array
  char n[maxChar];
  for (uint8_t j = 0; j < maxChar; j ++) {
    n[j] = ' ';
  }

  //pointer to blank array
  char *k = n;
  uint8_t totalLength = scrollLength + 1;
  while (scrollLength) {
    for ( uint8_t step = 0; step < 10 + 1; step++ ) {
      // step though each column of the 1st char for smooth scrolling
      cli();
      //copy the same char in the new array 9 chars ahead  top/bottom
      for (uint8_t j = 0; j < totalLength - scrollLength; j++) {
        n[j + 5] = m[j];
        n[j + 10] = m[j];
      }

      sendStringBig( k, step);
      sei();
    }
    k++;
    scrollLength--;
  }

  sei();
  setsDel(100);
  show();
}

// Show the passed string. The last letter of the string will be in the rightmost pixels of the display.
// Skip is how many cols of the 1st char to skip for smooth scrolling
static inline void sendString( const char *s, uint8_t skip,  const uint8_t r, const uint8_t g,  const uint8_t b ) {
  unsigned int l = PIXELS / (5 + 1);
  sendChar(' ', skip, rVT, gVT, bVT);       // First char is special case becuase it can be stepped for smooth scrolling

  uint8_t currentChar = 0;
  while ( *(++s) && l--) {
    if (currentChar < 10) {  //last char top row
      if (currentChar == 9) {
        sendChar(' ', 00, rVT, gVT, bVT );
      }
      else {
        sendChar(*s, 00, rVT, gVT, bVT );
      }
    }
    else {
      if (currentChar == 19) { //last char bottom row
        sendChar(' ', 00, rVB, gVB, bVB );
      }
      else {
        sendChar(*s, 00, rVB, gVB, bVB );
      }
    }
    currentChar++;
  }
  show();
}


  static inline void sendStringDual( const char *s, uint8_t skip,  const uint8_t r, const uint8_t g,  const uint8_t b) {
  unsigned int l = PIXELS / (FONT_WIDTH + INTERCHAR_SPACE);
  sendChar(*s, skip, r, g, b );       // First char is special case becuase it can be stepped for smooth scrolling

  uint8_t currentChar = 0;//which character are tring to display.  Need to manipulate so we don't have half chars
  while ( *(++s) && l--) {
    if (currentChar < 10) {  //last char top row
      if (currentChar == 9) {
        sendChar(' ', 00, rVT, gVT, bVT );
      }
      else {
        sendChar(*s, 00, rVT, gVT, bVT );
      }

    }
    else {
      if (currentChar == 19) { //last char bottom row
        sendChar(' ', 00, rVB, gVB, bVB );
      }
      else {
        sendChar(*s, 00, rVB, gVB, bVB );
      }
    }
    currentChar++;
  }

  show();
  }

  void scrollAndPauseBottom( char *m, int scrollLength, char scrollPrevious) {
  //create a blank array
  char n[maxChar];
  //if scrollPrevious is a space just flash previous away
  if (scrollPrevious == ' ') {
    for (uint8_t j = 0; j < maxChar; j ++) {
      n[j] = ' ';
    }
  }
  //if scroll previous other then space copy previous to display and then scroll away
  else {
    for (uint8_t j = 0; j < 11; j ++) {
      n[j + 10] = ' ';
      n[j] = dataStringCopy[j];
    }
  }
  //create a pointer to new blank array
  char *k = n;
  uint8_t totalLength = scrollLength + 1;
  //how many characters should we scroll
  while (scrollLength) {
    for ( uint8_t step = 0; step < FONT_WIDTH + INTERCHAR_SPACE; step++ ) {
      // step though each column of the 1st char for smooth scrolling
      cli();
      //this blanks out the top row.  this is why there is not chars scrolling across the top
      for (uint8_t j = 0; j < totalLength - scrollLength; j ++) {
        n[j + 20] = m[j];
        n[j + 10] = ' ';
      }
      sendString( k, step, rVB, gVB, bVB );
      sei();
    }
    k++;
    scrollLength--;
  }

  //if not scrolling the text off, re-draw the display to capture the last character
  //if we remove the padding, we might be able to alter the number is the for loop
  if (!sT) {
    cli();
    for (uint8_t j = 0; j < 10; j++) {
      sendChar(' ', 00, rVT, gVT, bVT );
    }
    for (uint8_t j = 0; j < 10; j++) {
      sendChar(m[j], 00, rVB, gVB, bVB );
    }
  }
  sei();
  setsDel(100);
  show();
  }

  void scrollAndPauseBig(char *m, uint8_t scrollLength, char scrollPrevious) {
  //create a blank array
  char n[maxChar];
  //if scrollPrevious was a space then just flash off what was on screen
  if (scrollPrevious == ' ') {
    for (uint8_t j = 0; j < maxChar; j ++) {
      n[j] = ' ';
    }
  }
  //if not space, copy the previous message and scroll that off
  //current only scroll off last message if big....can add more 'ifs' to scroll other previous
  else {
    for (uint8_t j = 0; j < 5; j ++) {
      n[j] = dataStringCopy[j];
      n[j + 5] = dataStringCopy[j];
    }
  }

  char *k = n;
  uint8_t totalLength = scrollLength + 1;
  while (scrollLength) {
    for ( uint8_t step = 0; step < BIG_FONT_WIDTH + INTERCHAR_SPACE; step++ ) {
      // step though each column of the 1st char for smooth scrolling
      cli();
      //copy same char 9 chars apart
      for (uint8_t j = 0; j < totalLength - scrollLength; j++) {
        n[j + 5] = m[j];
        n[j + 10] = m[j];
      }
      sendStringBig( k, step);
      sei();
    }
    k++;
    scrollLength--;
  }

  cli();
  //displayBigFromWeb(m); dont know just commented out the refferance
  sei();
  show();
  }

  void scrollAndPauseDual(char *m, uint8_t scrollLength, char scrollPrevious) {

  char n[maxChar];
  //if scrollPrevious is a space just flash previous away
  if (scrollPrevious == ' ') {
    for (uint8_t j = 0; j < maxChar; j ++) {
      n[j] = ' ';
    }
  }
  //if scroll previous other then space copy previous to display and then scroll away
  else {
    for (uint8_t j = 0; j < numSmChar - 1; j ++) {
      n[j] = dataStringCopy[j];
    }
  }

  char *k = n;
  uint8_t totalLength = scrollLength + 1;
  while (scrollLength) {

    for ( uint8_t step = 0; step < FONT_WIDTH + INTERCHAR_SPACE  ; step++ ) {
      // step though each column of the 1st char for smooth scrolling

      cli();

      //copy to 18 characters in advance because input from webpage/results is not padded
      for (uint8_t j = 0; j < totalLength - scrollLength; j++) {
        n[j + 10] = m[j];
        n[j + 20] = m[j + 10];
      }

      sendStringDual( k, step, rVT, gVT, bVT);

      sei();
    }

    k++;
    scrollLength--;

  }
  displaySmallText(m, 15);
}



void scrollAndPauseTop( char *m, uint8_t scrollLength, char scrollPrevious) {
  char n[maxChar];
  //if scrollPrevious is a space just flash previous away
  if (scrollPrevious == ' ') {
    for (uint8_t j = 0; j < maxChar; j ++) {
      n[j] = ' ';
    }
  }
  //if scroll previous other then space copy previous to display and then scroll away
  else {
    for (uint8_t j = 0; j < 11; j ++) {
      n[j] = ' ';
    //  n[j + 18] = dataStringCopy[j];
    }
  }

  //create a pointer to new blank array
  char *k = n;
  uint8_t totalLength = scrollLength + 1;
  while (scrollLength) {
    for ( uint8_t step = 0; step < 5 + 1  ; step++ ) {
      // step though each column of the 1st char for smooth scrolling
      cli();
      for (uint8_t j = 0; j < totalLength - scrollLength; j ++) {
        n[j + 10] = m[j];
        n[j + 20] = ' ';
      }

      sendString( k, step, rVT, gVT, bVT );
      sei();
    }
    k++;
    scrollLength--;
  }
  //if not scrolling the text off, re-draw the display to capture the last character
  //if we remove the padding, we might be able to alter the number is the for loop
  if (!sT) {
    cli();
    for (uint8_t j = 0; j < 10; j++) {
      sendChar(m[j], 00, rVT, gVT, bVT );
    }
    for (uint8_t j = 0; j < 10; j++) {
      sendChar(' ', 00, rVT, gVT, bVT );
    }
  }

  sei();
  setsDel(100);
  show();
}

//STATIC DISPLAYING
//only display what is in dataString
static inline void displaySmallText(char *p, uint8_t numClr2) {
  cli();
  //+10 is to cover for "half chars like periods and :"
  for (uint8_t j = 0; j < numSmChar + 10; j++) {
    if (j < 10) {
      sendChar(p[j], 00, rVT, gVT, bVT );
    }
    else if (j < numClr2) {
      if (p[j] == '.') {
        sendRowRGB( 0x06, rVB, gVB, bVB);
        sendRowRGB( 0x06, rVB, gVB, bVB);
        sendRowRGB( 0x00, rVB, gVB, bVB);
      }
      else if (p[j] == '*') {
        sendRowRGB( 0x00, rVB, gVB, bVB);
        sendRowRGB( 0x00, rVB, gVB, bVB);
        sendRowRGB( 0x00, rVB, gVB, bVB);
        sendRowRGB( 0x00, rVB, gVB, bVB);
      }
      else {
        sendChar(p[j], 00, rVB, gVB, bVB );
      }
    }
    else {
      if (p[j] == '.') {
        sendRowRGB( 0x06, rVR, gVR, bVR);
        sendRowRGB( 0x06, rVR, gVR, bVR);
        sendRowRGB( 0x00, rVR, gVR, bVR);
      }
      else if (p[j] == '*') {
        sendRowRGB( 0x00, rVR, gVR, bVR);
        sendRowRGB( 0x00, rVR, gVR, bVR);
        sendRowRGB( 0x00, rVR, gVR, bVR);
        sendRowRGB( 0x00, rVR, gVR, bVR);
      }
      else {
        sendChar(p[j], 00, rVR, gVR, bVR );
      }
    }
  }

  sei();
  show();
}

static inline void displayBigFromWeb(char *p) {
  uint8_t numChar = cC - 1;
  uint8_t numOfSpecial = 0;
  uint8_t extraRow = 0;
  for (uint8_t j = numChar; j < 5; j++) {
    p[numChar] = ' ';
    numChar ++;
  }

  for (uint8_t j = 0; j < numChar; j++) {
    if (p[j] == '.' || p[j] == ':' || p[j] == '*') {
      numOfSpecial++;
    }
  }

  if (numOfSpecial == 0) {
    numChar = 5;
  }
  else if (numOfSpecial == 1) {
    if (numChar < 6) {
      extraRow = 11;
    }
    else if (numChar == 6) {
      extraRow = 0;
    }
    else if (numChar == 7) {
      extraRow = 0;
    }

    if (numChar > 6) {
      numChar = 6;
    }
  }
  else if (numOfSpecial == 2) {
    if (numChar < 6) {
      extraRow = 17;
    }
    else if (numChar == 6) {
      extraRow = 6;
    }
    else if (numChar == 7) {
      extraRow = 6;
    }
    if (numChar > 6) {
      numChar = 6;
    }
  }
  else if (numOfSpecial == 3) {
    if (numChar < 6) {
      extraRow = 23;
    }
    else if (numChar == 6) {
      extraRow = 12;
    }
    else if (numChar == 7) {
      extraRow = 1;
    }
  }
  else if (numOfSpecial == 4) {
    if (numChar < 6) {
      extraRow = 29;
    }
    else if (numChar == 6) {
      extraRow = 18;
    }
    else if (numChar == 7) {
      extraRow = 7;
    }
  }

  cli();
  //if(extraSpaces == 0) {
  sendRowRGB( 0x00, 0, 0, 0xff );
  // }

  //TOP
  for (uint8_t j = 0; j < numChar; j++) {
    if (p[j] == '.') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == '*') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == ':') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x1c, rVT, gVT, bVT);
      sendRowRGB( 0x1c, rVT, gVT, bVT);
      sendRowRGB( 0x1c, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else {
      sendBigCharT(p[j], 0);
    }
  }

  //small font padding
  for (uint8_t j = 0; j < extraRow; j++) {
    sendRowRGB( 0x00, 0, 0, 0);
  }

  //big font padding
  if (numOfSpecial == 0) {
    for (uint8_t j = 0; j < 5; j++) {
      sendRowRGB( 0x00, 0, 0, 0);
    }
  }

  for (uint8_t j = 0; j < numChar; j++) {
    if (p[j] == '.') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x0e, rVT, gVT, bVT);
      sendRowRGB( 0x0e, rVT, gVT, bVT);
      sendRowRGB( 0x0e, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == '*') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == ':') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x38, rVT, gVT, bVT);
      sendRowRGB( 0x38, rVT, gVT, bVT);
      sendRowRGB( 0x38, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else {
      sendBigCharB(p[j], 0);
    }
  }
  //clear leftover
  for (uint8_t j = 0; j < 16; j++) {
    sendRowRGB( 0x00, 0, 0, 0xff );
  }

  sei();
  show();
}

static inline void displayBig1Special(char *p) {
  colorUpdate();
  delay(100);
  cli();
  //TOP
  for (uint8_t j = 0; j < 6; j++) {
    if (p[j] == '*') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == '.') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == ':') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x1c, rVT, gVT, bVT);
      sendRowRGB( 0x1c, rVT, gVT, bVT);
      sendRowRGB( 0x1c, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else {
      sendBigCharT(p[j], 0);
    }
  }

  for (uint8_t j = 0; j < 6; j++) {
    if (p[j] == '*') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == '.') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x0e, rVT, gVT, bVT);
      sendRowRGB( 0x0e, rVT, gVT, bVT);
      sendRowRGB( 0x0e, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == ':') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x38, rVT, gVT, bVT);
      sendRowRGB( 0x38, rVT, gVT, bVT);
      sendRowRGB( 0x38, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else {
      sendBigCharB(p[j], 0);
    }
  }
  //clear leftover
  for (uint8_t j = 0; j < 5; j++) {
    sendRowRGB( 0x00, 0, 0, 0xff );
  }

  sei();
  show();
}

static inline void displayHighJump(char *p) {
  uint8_t numChar = 7;
  uint8_t extraSpaces = 4;
  cli();
  sendRowRGB( 0x00, 0, 0, 0xff );
  sendBigCharT(p[0], 0);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendBigCharT(p[2], 0);
  sendBigCharT(p[3], 0);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);

  //TOP
  for (uint8_t j = 4; j < numChar; j++) {
    if (p[j] == 'x') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == 'o') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == '-') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else if (p[j] == '_') {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
    else {
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
      sendRowRGB( 0x00, rVT, gVT, bVT);
    }
  }


  sendBigCharB(p[0], 0);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x0e, rVT, gVT, bVT);
  sendRowRGB( 0x0e, rVT, gVT, bVT);
  sendRowRGB( 0x0e, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendBigCharB(p[2], 0);
  sendBigCharB(p[3], 0);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);


  for (uint8_t j = 4; j < numChar; j++) {
    if (p[j] == 'x') {
      sendRowRGB( 0x22, rVR, gVR, bVR);
      sendRowRGB( 0x14, rVR, gVR, bVR);
      sendRowRGB( 0x08, rVR, gVR, bVR);
      sendRowRGB( 0x14, rVR, gVR, bVR);
      sendRowRGB( 0x22, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
    }
    else if (p[j] == 'o') {
      sendRowRGB( 0x1c, rVR, gVR, bVR);
      sendRowRGB( 0x22, rVR, gVR, bVR);
      sendRowRGB( 0x22, rVR, gVR, bVR);
      sendRowRGB( 0x22, rVR, gVR, bVR);
      sendRowRGB( 0x1c, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
    }
    else if (p[j] == '-') {
      sendRowRGB( 0x08, rVR, gVR, bVR);
      sendRowRGB( 0x08, rVR, gVR, bVR);
      sendRowRGB( 0x08, rVR, gVR, bVR);
      sendRowRGB( 0x08, rVR, gVR, bVR);
      sendRowRGB( 0x08, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
    }
    else if (p[j] == '_') {
      sendRowRGB( 0x02, rVR, gVR, bVR);
      sendRowRGB( 0x02, rVR, gVR, bVR);
      sendRowRGB( 0x02, rVR, gVR, bVR);
      sendRowRGB( 0x02, rVR, gVR, bVR);
      sendRowRGB( 0x02, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
    }
    else {
      sendRowRGB( 0x00, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
      sendRowRGB( 0x00, rVR, gVR, bVR);
    }
  }
  //clear leftover
  for (uint8_t j = 0; j < 5; j++) {
    sendRowRGB( 0x00, 0, 0, 0xff );
  }

  sei();
  show();
}

static inline void displayDayTime(char *p) {
  uint8_t numChar = 7;
  uint8_t extraSpaces = 4;

  cli();

  //TOP
  sendBigCharT(p[0], 0);
  sendBigCharT(p[1], 0);
  sendRowRGB( 0x1c, rVT, gVT, bVT);
  sendRowRGB( 0x1c, rVT, gVT, bVT);
  sendRowRGB( 0x1c, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendBigCharT(p[3], 0);
  sendBigCharT(p[4], 0);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);

  //BOTTOM
  sendBigCharB(p[0], 0);
  sendBigCharB(p[1], 0);
  sendRowRGB( 0x38, rVT, gVT, bVT);
  sendRowRGB( 0x38, rVT, gVT, bVT);
  sendRowRGB( 0x38, rVT, gVT, bVT);
  sendRowRGB( 0x00, rVT, gVT, bVT);
  sendBigCharB(p[3], 0);
  sendBigCharB(p[4], 0);
  if (cVar == 'T') {
    sendChar('P', 00, rVB, gVB, bVB );
  }
  else {
    sendChar('A', 00, rVB, gVB, bVB );
  }
  sendChar('M', 00, rVB, gVB, bVB );

  for (uint8_t j = 0; j < 5; j++) {
    sendRowRGB( 0x00, 0, 0, 0xff );
  }

  sei();
  show();
}

//given time in milliseconds....
static inline void displayTimeToString(int t) {
  int min = (t / 60);
  int secs = (t - min * 60);
  int tens = (secs / 10);

  secs = (secs - tens * 10);
  int tmin = min / 10;
  char t_min = tmin + '0';
  char c_min = (min - tmin * 10) + '0';
  char c_tens = tens + '0';
  char c_secs = secs + '0';

  char timeAsChar[6];
  timeAsChar[0] = ' ';

  if (tmin > 0) {
    timeAsChar[1] = t_min;
  }
  else {
    timeAsChar[1] = ' ';
  }
  if (min > 0) {
    timeAsChar[2] = c_min;
    timeAsChar[3] = ':';
  }
  else {
    timeAsChar[2] = ' ';
    timeAsChar[3] = '*';
  }
  if ( tens == 0 && min == 0) {
    timeAsChar[4] = ' ';
  }
  else {
    timeAsChar[4] = c_tens;
  }
  timeAsChar[5] = c_secs;
  displayBig1Special(timeAsChar);
  checkInputs();
}

static inline void checkInputs(){
  checkRemote();
  //checkSerial();
}

static inline void resetCdTime(uint8_t *t) {
  cdTime = t[0] * 1000 + t[1] * 100 + t[2] * 10 + t[3];
  cdDiff = cdTime;
}

static inline void convertTimeToMs(uint8_t *t) {
  cdTime = t[0] * 600 + t[1] * 60 + t[2] * 10 + t[3];
  cdDiff = cdTime;
}
//kevinkevin
static inline void reStartCd() {
  cdTime = cdDiff;
  rtcStart = rtc.now();
  cdR = true;
  countDown();
}

static inline void countDown() {
  setsDel(200);
  rtcEnd = rtc.now();
  cdDiff = rtcStart.secondstime() + cdTime - rtcEnd.secondstime();
  //kevinkevin  !cdOnly
  if (dCD) {
    if (cdOnly) {
      displayTimeToString(cdDiff);
    }
    else {
      if (cdDiff % 3 == 0 || cdDiff < 16) {
        displayTimeToString(cdDiff);
      }
      else {
        displaySmallText(msg20Copy, 15);
      }
    }
  }

  if (cdDiff <= 0) {
    cdR = false;
    dCD = false;
  }

  checkInputs();
}

void pauseCountdown() {
  cdR = false;
  dCD = false;
}

//kevinkevin
void displayCountdown() {
  setSR(0);
  dCD = true;
}

void cdModeSelect() {
  cdOnly = !cdOnly;
  if (cdOnly) {
    displaySmallText("Countdown Only Mode ", 30);
  }
  else {
    displaySmallText("Countdown & Athletes", 30);
  }
}

//display the time as it is incrementing
void displayTimer(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4) {
  char formattedTime[6];
  formattedTime[0] = ' ';
  if (c1 == '0') {
    formattedTime[1] = ' ';
  }
  else {
    formattedTime[1] = c1;
  }

  if (c2 == '0' && c1 == '0') {
    formattedTime[2] = ' ';
    formattedTime[3] = '*';
  }
  else {
    formattedTime[2] = c2;
    formattedTime[3] = ':';
  }

  if (c3 == '0' && c2 == '0' && c1 == '0') {
    formattedTime[4] = ' ';
  }
  else {
    formattedTime[4] = c3;
  }

  formattedTime[5] = c4;

  displayBig1Special(formattedTime);
}

//works with displayTime to keep the time running/displaying
void showStopWatch() {
  while (subR == 13) {
    if (tSed) {
      cT = millis() - clT;

      rtcEnd = rtc.now();
      timeDiff = rtcEnd.secondstime() - rtcStart.secondstime();

      if (tS < 60) {
        tSC = timeDiff - (tM * 60);
        if (tSC != tS) {
          clT = millis();
          tS = tSC;
        }
      }
      else {
        tM++;
        tS = 0;
        cT = 0;
      }

      //left to right
      uint8_t char1 = ((tM / 10U) % 10) + '0';
      uint8_t char2 = ((tM / 1U) % 10) + '0';
      uint8_t char3 = ((tS / 10U) % 10) + '0';
      uint8_t char4 = ((tS / 1U) % 10) + '0';
      uint8_t char5 = ((cT / 100U) % 10) + '0';
      uint8_t char6 = ((cT / 10U) % 10) + '0';

      if (char3 == '6') {
        char3 = '0';
        if (char2 == 57) {
          char2 = '0';
          char1++;
        }
        else {
          char2++;
        }
      }
      checkInputs();
      displayTimer(char1, char2, char3, char4);
    }
    checkInputs();
  }
}

void startStopWatch() {
  tSed = true;
  rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));//year, month, day, hour, min, sec
  rtcStart = rtc.now();
  clT = millis();
  tS = 0;
  tM = 0;
  setSR(13);
  setsDel(10);
}

void stopStopWatch() {
  tSed = false;
  setSR(0);
}

uint8_t checkFrench(uint8_t bR) {

  if (bR >= 192 && bR <= 197) {
    bR = 65;//A 192-197
  }
  else if (bR == 199) {
    bR = 67;//C 199
  }
  else if (bR == 208) {
    bR = 68;//D 208
  }
  else if (bR >= 200 && bR <= 203) {
    bR = 69;//E 200-203
  }
  else if (bR >= 204 && bR <= 207) {
    bR = 73;//I 204-207
  }
  else if (bR == 209) {
    bR = 78;//N 209
  }
  else if (bR >= 210 && bR <= 214) {
    bR = 79;//O 210-214
  }
  else if (bR >= 217 && bR <= 220) {
    bR = 85;//U 217-220
  }
  else if (bR == 221) {
    bR = 89;//Y 221
  }
  else if (bR >= 224 && bR <= 229) {
    bR = 97;//a 224-229
  }
  else if (bR == 231) {
    bR = 99;//c 231
  }
  else if (bR >= 232 && bR <= 235) {
    bR = 101;//e 232-235
  }
  else if (bR >= 236 && bR <= 239) {
    bR = 105;//i 236-239
  }
  else if (bR == 241) {
    bR = 110;//n 241
  }
  else if (bR >= 242 && bR <= 246) {
    bR = 111;//o 242-246
  }
  else if (bR >= 249 && bR <= 252) {
    bR = 117;//u 249-252
  }
  else if (bR == 253 || bR == 255) {
    bR = 121;//y 253-255
  }

  return bR;
}

static inline void clearMsg20() {
  for (uint8_t i = 0; i < numSmChar; i++) {
    msg20[i] = ' ';
  }
}

static inline void clearMsg6() {
  for (uint8_t i = 0; i < numBgChar; i++) {
    msg6[i] = ' ';
  }
}

static inline void clearMsgHjLg() {
  for (uint8_t i = 0; i < numHjLg; i++) {
    msgHjLg[i] = ' ';
  }
}

//rEM
static inline void cEMS() {
  rEM = !rEM;
  delay(200);
  if (rEM) {
    cdTS[0] = 'p';
    cdTS[1] = '_';
    cdTS[2] = '_';
    cdTS[3] = ':';
    cdTS[4] = '_';
    cdTS[5] = '_';
    cdTimeIn[0] = 0;
    cdTimeIn[1] = 0;
    cdTimeIn[2] = 0;
    cdTimeIn[3] = 0;
    rEC = 1;
  }
  else {
    cdTimeIn[0] = cdTS[1] - 48;
    cdTimeIn[1] = cdTS[2] - 48;
    cdTimeIn[2] = cdTS[4] - 48;
    cdTimeIn[3] = cdTS[5] - 48;

    for (uint8_t j = 0; j < 4; j++) {
      if (cdTimeIn[j] < 0 || cdTimeIn[j] > 9) {
        cdTimeIn[j] = 0;
      }
    }

    if (cdTimeIn[1] == 9 && cdTimeIn[2] > 5 ) {
      cdTimeIn[2] = 5;
    }
    delay(200);
    setSR(15);
  }
}
static inline void setCD(uint8_t n) {
  if (!rEM) {
    cdTimeIn[0] = 0;
    cdTimeIn[1] = n;
    cdTimeIn[2] = 0;
    cdTimeIn[3] = 0;

    if (n == 4) {
      cdTimeIn[1] = 1;
      cdTimeIn[2] = 3;
    }
    setSR(15);
  }
  else {
    enterCDval(n + '0');

    //add space for colon
    if (rEC == 3) {
      rEC = rEC + 1;
    }
    //reset to the beginning of the string
    if (rEC == 6) {
      rEC = 1;
    }
    displayBig1Special(cdTS);
  }
}

static inline void enterCDval(char n) {
  delay(200);
  cdTS[rEC] = n;
  rEC = rEC + 1;
}

static inline void sendCDtoDisplay() {
  colorUpdate();
  convertTimeToMs(cdTimeIn);
  displayTimeToString(cdTime);
  setSR(0);;
}

static inline void cDbelow16() {
  if (vP) {
    digitalWrite(17, HIGH);
    digitalWrite(16, HIGH);
  }
  else {
    digitalWrite(17, LOW);
    digitalWrite(16, LOW);
  }
}

static inline void checkRemote() {
  // if less then 16 turn on relay
  if ((cdDiff < 16 && dCD) || externalCount) {
    if (!vP) {
      vP = !vP;
      cDbelow16();
    }
  }
  else {
    if (vP) {
      vP = !vP;
      cDbelow16();
    }
  }

  if (IrReceiver.decode()) { //if we have received an IR signal
    String remoteValue = String(IrReceiver.decodedIRData.decodedRawData, HEX).substring(0, 4);
    //  turn remote on or off
    if (remoteValue == "fd02") {
      rOO = !rOO;
      if (rOO) {
        displayBig1Special("Rm.On ");
      }
      else {
        displayBig1Special("Rm.Off");
      }
      delay(500);
    }

    if (rOO) {
      //enter
      if (remoteValue == "9768") {
        remoteValue = '0';
        cEMS();
      }

      if (rEM) {
        //0
        if (remoteValue == "ee11") {
          enterCDval('0');
        }
        //1
        else if (remoteValue == "fb04")  {
          enterCDval('1');
        }
        //2
        else if (remoteValue == "fa05") {
          enterCDval('2');
        }
        //3
        else if (remoteValue == "f906") {
          enterCDval('3');
        }
        //4
        else if (remoteValue == "f708") {
          enterCDval('4');
        }
        //5
        else if (remoteValue == "f609") {
          enterCDval('5');
        }
        //6
        else if (remoteValue == "f50a") {
          enterCDval('6');
        }
        //7
        else if (remoteValue == "f30c") {
          enterCDval('7');
        }
        //8
        else if (remoteValue == "f20d") {
          enterCDval('8');
        }
        //9
        else if (remoteValue == "f10e") {
          enterCDval('9');
        }
        //add space for colon
        if (rEC == 3) {
          rEC = rEC + 1;
        }
        //reset to the beginning of the string
        if (rEC == 6) {
          rEC = 1;
        }
        displayBig1Special(cdTS);

        remoteValue = '0';
      }
      else {
        //1
        if (remoteValue == "fb04") {
          setCD(1);
        }
        //2
        else if (remoteValue == "fa05") {
          setCD(2);
        }
        //3
        else if (remoteValue == "f906") {
          setCD(3);
        }
        //4
        else if (remoteValue == "f708") {
          setCD(4);
        }
        //5
        else if (remoteValue == "f609") {
          setCD(5);
        }
        //colour change
        else if (remoteValue == "9f60") {
          patChgL();
        }
        //start
        else if (remoteValue == "b847") {
          setSR(16);
        }
        //STOP
        else if (remoteValue == "b946") {
          pauseCountdown();
          setSR(17);
        }
        //restart
        else if (remoteValue == "b649") {
          cdR = false;
          dCD = false;
          setSR(15);
        }
        // show countdown
        else if (remoteValue == "946b") {
          displayCountdown();
        }
        else if (remoteValue == "9768") {
          displayCountdown();
        }
        else if (remoteValue == "eb14") {
          startStopWatch();
        }
        else if (remoteValue == "936c") {
          stopStopWatch();
        }
        else if (remoteValue == "e916") {
          setSR(12);
          setsDel(10);
        }
        else if (remoteValue == "ea15") {
          setSR(12);
          setsDel(10);
        }
        else if (remoteValue == "f807") {
          brightUp();
        }
        else if (remoteValue == "f40b") {
          brightDown();
        }
        else if (remoteValue == "8679") {
          loadDefault();
        }
        else if (remoteValue == "dc23") {
          displaySmallText(ipString, 30);
          setSR(0);
        }
        else if (remoteValue == "ec13") {
          cdModeSelect();
        }
        else if (remoteValue == "f00f") {
          displaySmallText("                      ", 30);
        }
      }
    }
    remoteValue = "0000";
  }
  IrReceiver.resume();
}

//serial monitoring function
// had some seriallyncs refferances static inline void checkSerial() {


//  if (subR != 0) {
    //dCD = false;
  //}
//}

//runs when device is powered on
void setup() {
  delay(3000);
  Serial.begin(9600);
  rtc.begin();
  clearDataString();
  PIXEL_DDRC |= fullBits;  //have to set all bits cause using pins 0 and 1 as Serial input and output       // Set used pins to output mode
  IrReceiver.begin(10, ENABLE_LED_FEEDBACK); 
  pinMode(17, OUTPUT);  //vPin1
  pinMode(16, OUTPUT);  //vPin2
}

//calls serial monitor to know what to do and breaks into sub routines.
void loop() {

  checkInputs();

  if (subR != 0) {
    dCD = false;
  }
  //kevinkevinkevin
  if (dCD) {
    if (cdR) {
      countDown();
    }
    else {
      displayTimeToString(cdDiff);
    }
  }
  //load default message
  if (subR == 1) {
    colorUpdate();
    loadDefault();
  }
  //big scrolling
  else if (subR == 4) {
    setsDel(4000);
    colorUpdate();
    sT = true;
    scrollBig(dataString, cC + 5);
    sT = false;
    loadDefault();

    if (tSed) {
      setSR(12);
    } else {
      setSR(0);
    }
  }
  
    //scrolling string along the top
    else if (subR == 2) {
    setsDel(5000);
    colorUpdate();
    sT = true;
    scrollAndPauseTop(dataString, cC + 10, ' ');
    sT = false;
    loadDefault();

    if (tSed) {
      setSR(12);
    } else {
      setSR(0);;
    }

    }
    //scrolling accross the bottom
    else if (subR == 3) {
    setsDel(5000);
    colorUpdate();
    sT = true;
    scrollAndPauseBottom(dataString, cC + 10, ' ');
    sT = false;
    loadDefault();

    if (tSed) {
      setSR(12);
    }
    else {
      setSR(0);;
    }

    }
    //scroll and pause along the top
    else if (subR == 5) {
    setsDel(4000);
    colorUpdate();
    scrollAndPauseTop(dataString, 10, cVar);
    setSR(0);;
    }
    //scroll and pause along the bottom
    else if (subR == 6) {
    setsDel(4000);
    colorUpdate();
    scrollAndPauseBottom(dataString, 10, cVar);
    setSR(0);;
    }
    //scroll and pause big
    else if (subR == 7) {
    setsDel(3000);
    colorUpdate();
    sT = true;
    scrollAndPauseBig(dataString, 5, cVar);
    sT = false;
    setSR(0);;
    }
    //scroll and pause along both rows
    else if (subR == 8) {
    setsDel(3000);
    colorUpdate();
    sT = true;
    scrollAndPauseDual(dataString, 10, cVar);
    sT = false;
    setSR(0);;
    }
  
  //display static text
  //kevinkevin
  else if (subR == 9) {
    setsDel(200);
    colorUpdate();
    if (cVar == '1') {
      displaySmallText(dataString, 30);
    }
    else {
      displaySmallText(dataString, 14);
    }
    setSR(0);
  }
  //display Big Text
  else if (subR == 10) {
    setsDel(200);
    colorUpdate();
    if (cVar == 'H') {
      displayHighJump(dataString);
    }
    else if (cVar == 'T' || cVar == 't') {
      displayDayTime(dataString);
    }
    //referance to FromWeb not sure if neccesary or just a catch else statement
//    else {
  //    displayBigFromWeb(dataString);
    //}
    //setSR(0);
  }
  else if (subR == 11) {
    setsDel(200);
    colorUpdate();
    displaySmallText(dataString, 15);
    setSR(0);
  }
  //display timer
  else if (subR == 12) {
    colorUpdate();
    displayBig1Special("    *0");
    setSR(13);
  }
  //load the stopwatch
  else if (subR == 13) {
    colorUpdate();
    showStopWatch();
  }
  else if (subR == 14) {
  //  colorUpdate();
  //  setSR(0);
  }
  else if (subR == 15) {
    sendCDtoDisplay();
  }
  else if (subR == 16) {
    colorUpdate();
    reStartCd();
    displayCountdown();
  }
  //STOP countdown
  else if (subR == 17) {
    colorUpdate();
    pauseCountdown();
    //display what was last shown, vertical jump format or regular format
    if (!cdOnly) {
      if (cVar == 'v') {
        setsDel(200);
        colorUpdate();
        displaySmallText(msg20Copy, 15);
        setSR(0);
      }
      else {
        setsDel(200);
        colorUpdate();
        displaySmallText(msg20Copy, 14);
        setSR(0);
      }
    }
    setSR(0);
    //if cdOnly don't need to change what was showing
  }
//  else if (subR == 21) {
  //  sendMsg20(); no refferance
//  }
  //pass horizontal or throw
  else if (subR == 22) {
    msg6[0] = '*';
    msg6[1] = 'P';
    msg6[2] = 'A';
    msg6[3] = 'S';
    msg6[4] = 'S';
    msg6[5] = ' ';
    //sendMsg6(); 
  }
  else if (subR == 23) {
    msg6[0] = 'F';
    msg6[1] = 'A';
    msg6[2] = 'U';
    msg6[3] = 'L';
    msg6[4] = 'T';
    msg6[5] = '*';
    //sendMsg6();
  }
  //high jump cleared
  else if (subR == 24) {
    //sendHjLg();
  }
  //high jump cleared
  else if (subR == 25) {
    //sendHjLg();
  }
  else if (subR == 26) {
    //sendHjLg();
  }
  else if (subR == 27) {
    //sendHjLg();
  }
  //vertical jump attempt 1
  else if (subR == 28) {
    //sendMsg20();
  }
  else if (subR == 29) {
    //sendHjLg();
  }
  //vertical jump attempt 2
  else if (subR == 30) {
    //sendMsg20();
  }
  //vertical jump attempt 3
  else if (subR == 31) {
    //sendMsg20();
  }
  //horizontal jump or throw result large
  else if (subR == 32) {
    //sendMsg6();
  }
  else {
    // BTSerial.write("error");
  }

}