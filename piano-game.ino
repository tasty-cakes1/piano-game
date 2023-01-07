#include "ArcadePiano.h"

// Key Switch Pins
#define KEY_1_PIN 11
#define KEY_2_PIN 10
#define KEY_3_PIN 9
#define KEY_4_PIN 8

ArcadePiano ap = ArcadePiano(KEY_1_PIN,KEY_2_PIN,KEY_3_PIN,KEY_4_PIN);

void setup() {
  ap.begin();
}

void loop()
{
  ap.printHighScores();
  ap.startGame();
}
