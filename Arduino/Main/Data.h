#ifndef DATA_H
#define DATA_H

#include <Arduino.h>
#include "RTClib.h"

extern bool rOO; //is the remote on or off
extern bool rEM; //are you entering a manual countdown time "rEM"
extern bool vP; //should we be sending voltage out
extern uint8_t rEC; //remote enter count...used to determine
extern uint8_t vPin;
extern uint8_t vPin2;

extern RTC_DS3231 rtc;
extern DateTime rtcStart;
extern DateTime rtcEnd;

extern bool flStarted;

extern bool cSC;  //capture Scroll Char
extern bool sT; //scrolling text if text is scrolling....keep scrolling do not scroll and pause
extern bool cdOnly; //countdown only mode.  displays stopped time.

extern uint8_t clrPat; 
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
extern uint8_t br[10];
//TOP COLOUR
extern uint8_t brLvl;
extern uint8_t rVT;
extern uint8_t gVT;
extern uint8_t bVT;
//BOTTOM COLOUR
extern uint8_t rVB;
extern uint8_t gVB;
extern uint8_t bVB;
//RESULT section COLOUR
extern uint8_t rVR;
extern uint8_t gVR;
extern uint8_t bVR;

//TIMINING SPECIFIC variables
extern uint8_t tS;  //timer Seconds
extern uint8_t tSC;//timer Seconds Current
extern uint8_t tM;  //timer minuutes
extern char cdTS[6]; //count down time String
extern bool tSed; //timer started  - used to control what to show based on if the clock is running or not
extern unsigned long clT; //cleared time
extern unsigned long cT; // current time
extern uint8_t cdTimeIn[4];
extern unsigned long cdTime;
extern unsigned long cdDiff;
extern unsigned long timeDiff;
extern bool cdR; //countdown running
extern bool dCD; //display countdown
extern bool externalCount;  //countdown from fieldlynx

//TEXT section
extern uint8_t cC;  //char count controls the number of chars that are in the input set
extern int B;  //byte recieved.  stores the incoming byte
extern uint8_t subR;  //sub routine used to determine what routine to display, timer, scroll and pause etc.
extern int sDel; // show delay //number of miliseconds to scroll across 20 chars
extern const uint8_t maxChar;  //max number of characters that scroll accross the display
extern const uint8_t numSmChar;
extern char msg20[];
extern char msg20Copy[];
extern char dataString[];//the main string of data
extern char dataStringCopy[]; //can be used to display on the screen before scrolling off
extern char ipString[]; //can be used to display on the screen before scrolling off
extern char cVar ; //control varaiable.  leading variable passed to the display to control feature options

extern bool isVertical ;
extern char *strings[7];
extern char *ptr;
extern char *nPtr;
extern char *name[2];

#define PIXELS 120  //number of pixels in one strip of lights

#define PIXEL_PORTC  PORTC// Port of the pin the pixels are connected to  //sets what the pins are
#define PIXEL_DDRC  DDRC   // Port of the pin the pixels are connected to  //sets what the pins do

extern const uint8_t fullBits;
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

extern const uint8_t numBgChar;
extern char msg6[];
//char msg6Copy[numBgChar];

extern const uint8_t numHjLg;
extern char msgHjLg[];

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
extern const uint8_t Font10x14t_[] PROGMEM;
//bottom of the 10 wide 14 high font
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // `
// 0x00, 0x80, 0xc0, 0x20, 0xfe, 0x08, 0x10, 0x60, 0x40, 0x00, // Rose
extern const uint8_t Font10x14b[] PROGMEM;

//standard 5x7 font
extern const uint8_t Font5x7[] PROGMEM;


// Functions
static inline void sendBitx8(const uint8_t row, const uint8_t colorbyte, const uint8_t onBits);
static inline void sendRowRGB(uint8_t row,  uint8_t r,  uint8_t g,  uint8_t b);
void show();
void setsDel(int ms);
#endif // DATA_H