#include <Adafruit_LEDBackpack.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface for the LED Matrix Display
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES	4

// LED Matrix Pins
#define CLK_PIN   15  // or SCK
#define DATA_PIN  16  // or MOSI
#define CS_PIN    17  // or SS

// Key Switch Pins
#define KEY_1_PIN 11
#define KEY_2_PIN 10
#define KEY_3_PIN 9
#define KEY_4_PIN 8

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

#define CHAR_SPACING  1 // pixels between characters

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// I2C 7-Segment Display Backpack
Adafruit_7segment scoreTimer = Adafruit_7segment();

const byte KEY_PINS[] = {KEY_1_PIN,KEY_2_PIN,KEY_3_PIN,KEY_4_PIN};

byte key_list[8];   //circular buffer for next keys to press 
byte key_list_index=0;

byte key_presses=0;

bool start=false;
unsigned int score=0;
unsigned long startTime;

void stripe()
// Demonstrates animation of a diagonal stripe moving across the display
// with points plotted outside the display region ignored.
{
  const uint16_t maxCol = MAX_DEVICES*ROW_SIZE;
  const uint8_t	stripeWidth = 10;

  mx.clear();

  for (uint16_t col=0; col<maxCol + ROW_SIZE + stripeWidth; col++)
  {
    for (uint8_t row=0; row < ROW_SIZE; row++)
    {
      mx.setPoint(row, col-row, true);
      mx.setPoint(row, col-row - stripeWidth, false);
    }
    readKeys();
    if(key_presses!=0){
      start=true;
      mx.clear();
      return;
    }
    delay(DELAYTIME);
  }
}

void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
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

void scrollText(const char *p)
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

void printKeys() {
  mx.clear();
  for(int i=0;i<8;i++){
    for(int j=0;j<5;j++){
      mx.setPoint(7-i,key_list[((key_list_index+i))%8]*9+j,true);
    }
  }
}

void readKeys() {
  key_presses=0;
  for(int i=0;i<4;i++){
    if(!digitalRead(KEY_PINS[i])){      
      key_presses+=1<<i;
    }
  }
}

void nextKey() {
  key_list[key_list_index]=(byte)random(4);
  key_list_index=(key_list_index+1)%8;
}

void printFlash() {
  mx.clear();
  for(int i=0;i<4;i++){
    delay(500);
    mx.clear();
    delay(500);
    printKeys();
  }
}

void setup() {
  for(int i=0;i<4;i++){
    pinMode(KEY_PINS[i],INPUT_PULLUP);
  }
  scoreTimer.begin(0x70);
  scoreTimer.setBrightness(0);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY,0);
  randomSeed(analogRead(3));
  for(int i=0;i<8;i++) {
    key_list[i]=(byte)random(4);
  }
}

String getInitials() {
  mx.clear();
  char test[1]={char(16)};
  printText(0,0,test);
  char initials[]="   ";
  char temp[2];
  readKeys();
  while(key_presses!=1<<3){
    readKeys();
    for(int i=0;i<3;i++){
      if(key_presses==1<<i){
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
  return initials;
}

void loop()
{
  stripe();
  // readKeys();
  // if(key_presses){
  //   start=true;
  // }
  if(start){
    for(int i=3;i>0;i--){
      scoreTimer.print(i);
      scoreTimer.writeDisplay();
      delay(1000);
    }
    scoreTimer.print(score);
    scoreTimer.writeDisplay();
    printKeys();
    startTime=millis();
  }
  while(start){
    readKeys();
    for(int i=0;i<4;i++){
      if(key_presses==(1<<i)){
        if(key_list[key_list_index]==(3-i)){
          nextKey();
          printKeys();
          scoreTimer.print(++score);
          scoreTimer.writeDisplay();
          delay(20);
          while(key_presses!=0){
            readKeys();
          }
          delay(20);
          break;
        }
        else{
          printFlash();
          break;
        }  
      }
      if(i==3 && key_presses!=0){
        printFlash();
        break;
      }
    }
    if(int(60-(millis()-startTime)/1000)<=0){
      start=false;
      for(int i=0;i<3;i++){
        scoreTimer.clear();
        scoreTimer.writeDisplay();
        delay(500);
        scoreTimer.print(score);
        scoreTimer.writeDisplay();
        delay(500);
      }
      score=0;
    }
  }
}
