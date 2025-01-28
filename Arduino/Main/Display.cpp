#include "Display.h"

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

    for (uint8_t step = 0; step < FONT_WIDTH + INTERCHAR_SPACE; step++) {
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