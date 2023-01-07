// ArcadePiano.h
#pragma once

#include <EEPROM.h>

#include <Adafruit_LEDBackpack.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface for the LED Matrix Display
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES	4

// LED Matrix Pin
#define CS_PIN    17  // or SS

#define NUM_KEYS 4

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

#define CHAR_SPACING  1 // pixels between characters

class ArcadePiano {
  private:
    MD_MAX72XX mx;
    Adafruit_7segment scoreTimer;
    uint8_t keyPins[4];
    unsigned int highScores[3];
    char initials[3][4];
    void printKeys(byte * keyQueue,byte keyQueueStartIndex);
    void getInitials(char* buf);

  public:
    ArcadePiano(uint8_t key1Pin,uint8_t key2Pin,uint8_t key3Pin,uint8_t key4Pin);
    void clearHighScores();
    void printHighScores();
    void startGame();
    byte getKeyPresses();
    void printText(uint8_t modStart, uint8_t modEnd, char *pMsg);
    void scrollText(const char *p);
    void begin();
};