#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>
#include "Color.h"
#include "Data.h"
#include "Display.h"

static inline void resetCdTime(uint8_t *t);
static inline void convertTimeToMs(uint8_t *t);
static inline void reStartCd();
static inline void countDown();
void pauseCountdown();
void displayCountdown();
void cdModeSelect();
void displayTimer(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4);
void showStopWatch();
void startStopWatch();
void stopStopWatch();
uint8_t checkFrench(uint8_t bR);
static inline void clearMsg20();
static inline void clearMsg6();
static inline void clearMsgHjLg();
static inline void cEMS();
static inline void setCD(uint8_t n);
static inline void enterCDval(char n);
static inline void sendCDtoDisplay();
static inline void cDbelow16();

#endif // TIMER_H