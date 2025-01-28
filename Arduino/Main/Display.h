#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "Data.h"
#include "InputManager.h"

// To clear the LEDs
static inline void clear();
void setSR(uint8_t r);
void loadDefault();
void clearDataString();
void copyDataString();
void colorUpdate();
void clrMes();
void patChgR();
void patChgL();
void brightUp();
void brightDown();
static inline void sendChar(uint8_t c,  uint8_t skip, uint8_t r,  uint8_t g,  uint8_t b);
static inline void sendBigCharT(uint8_t c,  uint8_t skip);
static inline void sendBigCharB(uint8_t c,  uint8_t skip);
static inline void sendStringBig(const char *s, uint8_t skip);
static inline void scrollBig(char *m, uint8_t scrollLength);
static inline void sendString(const char *s, uint8_t skip,  const uint8_t r, const uint8_t g,  const uint8_t b);
static inline void sendStringDual(const char *s, uint8_t skip,  const uint8_t r, const uint8_t g,  const uint8_t b);
void scrollAndPauseBottom( char *m, int scrollLength, char scrollPrevious);
void scrollAndPauseBig(char *m, uint8_t scrollLength, char scrollPrevious);
void scrollAndPauseDual(char *m, uint8_t scrollLength, char scrollPrevious);
void scrollAndPauseTop( char *m, uint8_t scrollLength, char scrollPrevious);
static inline void displaySmallText(char *p, uint8_t numClr2);

static inline void displayBigFromWeb(char *p);
static inline void displayBig1Special(char *p);

static inline void displayHighJump(char *p);

// For time
static inline void displayDayTime(char *p);
static inline void displayTimeToString(int t);

#endif // DISPLAY_H