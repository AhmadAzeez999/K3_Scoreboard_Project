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