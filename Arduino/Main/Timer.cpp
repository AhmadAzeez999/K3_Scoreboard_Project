#include <Arduino.h>
#include "Timer.h"
#include "Color.h"
#include "Data.h"
#include "Display.h"

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
  setSR(0);
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