#include <IRremote.h>
#include "RTClib.h"
#include "Data.h"
#include "Display.h"
#include "Timer.h"
#include "Remote.h"
#include "Color.h"
#include "InputManager.h"

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