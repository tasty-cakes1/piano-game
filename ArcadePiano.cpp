// ArcadePiano.cpp
#include "ArcadePiano.h"


ArcadePiano::ArcadePiano(uint8_t key1Pin,uint8_t key2Pin,uint8_t key3Pin,uint8_t key4Pin) 
: mx(HARDWARE_TYPE, CS_PIN, MAX_DEVICES)
{
  keyPins[0]=key1Pin;
  keyPins[1]=key2Pin;
  keyPins[2]=key3Pin;
  keyPins[3]=key4Pin;
  for(int i=0;i<NUM_KEYS;i++){
    pinMode(keyPins[i],INPUT_PULLUP);
  }

  for(int i=0;i<3;i++) {
    EEPROM.get(i*(sizeof(highScores[0])+sizeof(initials[0])),highScores[i]);
    EEPROM.get(i*(sizeof(highScores[0])+sizeof(initials[0]))+sizeof(highScores[0]),initials[i]);
  }
}


void ArcadePiano::begin()
{
  scoreTimer.begin(0x70); 
  scoreTimer.setBrightness(0);

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY,0);
  
  randomSeed(analogRead(A3));
}


void ArcadePiano::startGame()
{
  unsigned int score=0;
  byte keyQueue[8];
  byte keyQueueStartIndex = 0;
  for(int i=0;i<8;i++) {
    keyQueue[i]=(byte)random(4);
  }
  for(int i=3;i>0;i--){
    scoreTimer.print(i);
    scoreTimer.writeDisplay();
    delay(1000);
  }
  scoreTimer.print(0);
  scoreTimer.writeDisplay();

  printKeys(keyQueue, keyQueueStartIndex);
  unsigned long startTime=millis();
  
  while(millis()-startTime<60000){
    byte keyPresses=getKeyPresses();
    for(int i=0;i<NUM_KEYS;i++){
      if(keyPresses==(1<<i)){
        if(keyQueue[keyQueueStartIndex]==(3-i)){
          keyQueue[keyQueueStartIndex]=(byte)random(4);
          keyQueueStartIndex=(keyQueueStartIndex+1)%8;
          printKeys(keyQueue, keyQueueStartIndex);
          scoreTimer.print(++score);
          scoreTimer.writeDisplay();
          delay(20);
          while(getKeyPresses()!=0);
          delay(20);
          break;
        }
        else{
          mx.clear();
          delay(500);
          for(int i=0;i<2;i++){
            printKeys(keyQueue,keyQueueStartIndex);
            delay(500);
            mx.clear();
            delay(500);
          }
          printKeys(keyQueue,keyQueueStartIndex);
          break;
        }  
      }
      if(i==3 && keyPresses!=0){
        mx.clear();
        delay(500);
        for(int i=0;i<2;i++){
          printKeys(keyQueue,keyQueueStartIndex);
          delay(500);
          mx.clear();
          delay(500);
        }
        printKeys(keyQueue,keyQueueStartIndex);
        break;
      }
    }
  }

  for(int i=0;i<3;i++){
    scoreTimer.clear();
    scoreTimer.writeDisplay();
    delay(500);
    scoreTimer.print(score);
    scoreTimer.writeDisplay();
    delay(500);
  }
  if(score>highScores[2]){
    scrollText("High Score!          ");
    char initial[4];
    getInitials(initial);
    if(score>highScores[0]){
      highScores[2]=highScores[1];
      highScores[1]=highScores[0];
      highScores[0]=score;
      strcpy(initials[2],initials[1]);
      strcpy(initials[1],initials[0]);
      strcpy(initials[0],initial);
    }
    else if(score>highScores[1]){
      highScores[2]=highScores[1];
      highScores[1]=score;
      strcpy(initials[2],initials[1]);
      strcpy(initials[1],initial);
    }
    else{
      highScores[2]=score;
      strcpy(initials[2],initial);
    }
    for(int i=0;i<3;i++) {
      EEPROM.put(i*(sizeof(highScores[0])+sizeof(initials[0])),highScores[i]);
      EEPROM.put(i*(sizeof(highScores[0])+sizeof(initials[0]))+sizeof(highScores[0]),initials[i]);
    }
  }
  else{
    mx.clear();
    while(getKeyPresses()==0);
    delay(20);
    while(getKeyPresses()!=0);
    delay(20);
  }
}


void ArcadePiano::clearHighScores()
{
  for(int i=0;i<3;i++){
    highScores[i]=0;
    EEPROM.put(i*(sizeof(highScores[0])+sizeof(initials[0])),highScores[i]);
  }
}


byte ArcadePiano::getKeyPresses() 
{
  byte keyPresses=0;
  for(int i=0;i<NUM_KEYS;i++){
    if(!digitalRead(keyPins[i])){      
      keyPresses+=1<<i;
    }
  }
  return keyPresses;
}


void ArcadePiano::printKeys(byte * keyQueue,byte keyQueueStartIndex) 
{
  mx.clear();
  for(int i=0;i<8;i++){
    for(int j=0;j<5;j++){
      mx.setPoint(7-i,keyQueue[((keyQueueStartIndex+i))%8]*9+j,true);
    }
  }
}


void ArcadePiano::printHighScores() 
{
  const uint16_t maxCol = MAX_DEVICES*ROW_SIZE;
  const uint8_t	stripeWidth = 10;
  while(true){
    for(int i=0;i<3;i++){
      if(highScores[i]==0){
        break;
      }
      mx.clear();
      for(int j=0;j<3;j++){
        char temp[2];
        temp[0]=initials[i][j];
        temp[1]='\0';
        printText(2-j,2-j,temp);
      }
      scoreTimer.print(highScores[i]);
      scoreTimer.writeDisplay();
      for(int j=0;j<2000;j++){
        delay(1);
        if(getKeyPresses()!=0){
          delay(100);
          unsigned long t=millis();
          while(getKeyPresses()==15){
            if(millis()-t>10000){
              mx.clear();
              printText(0,3,"Clear?");
            }
            if(millis()-t>15000){
              mx.clear();
              printText(0,3,"Clear!");
              delay(2000);
              clearHighScores();
              while(getKeyPresses()!=0);
              delay(20);
              continue;
            }
          }
          mx.clear();
          return;
        }
      }
      scoreTimer.clear();
      scoreTimer.writeDisplay();
    }
      
    mx.clear();

    for (uint16_t col=0; col<maxCol + ROW_SIZE + stripeWidth; col++)
    {
      for (uint8_t row=0; row < ROW_SIZE; row++)
      {
        mx.setPoint(row, col-row, true);
        mx.setPoint(row, col-row - stripeWidth, false);
      }
      if(getKeyPresses()!=0){
        mx.clear();
        return;
      }
      delay(DELAYTIME);
    }
  }
}


void ArcadePiano::getInitials(char* buf) {
  mx.clear();
  char arrow[2]={char(16),'\0'};
  printText(0,0,arrow);
  char initials[]="   ";
  char temp[2];
  while(getKeyPresses()!=1<<3){
    byte keyPresses=getKeyPresses();
    for(int i=0;i<3;i++){
      if(keyPresses==1<<i){
        if(initials[i]==' '){
          initials[i]='Z';
        }
        initials[i]=char((int(initials[i])-int('A')+1)%26+int('A'));
        temp[0]=initials[i];
        temp[1]='\0';
        printText(3-i,3-i,temp);
        delay(200);
      }
    }
  }
  delay(20);
  while(getKeyPresses()!=0);
  delay(20);

  strcpy(buf, initials);
}


void ArcadePiano::printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t   curLen;
  uint16_t  showLen;
  uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:	// display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}


void ArcadePiano::scrollText(const char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts

  mx.clear();

  while (*p != '\0')
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

    for (uint8_t i=0; i<=charWidth; i++)	// allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      if (i < charWidth)
        mx.setColumn(0, cBuf[i]);
      delay(DELAYTIME);
      // readKeys();
      // if(key_presses!=0){
      //   start=true;
      //   mx.clear();
      //   return;
      // }
    }
  }
}



