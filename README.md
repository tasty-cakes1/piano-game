# Mini Piano Arcade Game
Arduino code for a [Grand Piano Keys](https://www.baytekent.com/grand-piano-keys/) inspired mini arcade game.

Code is written for and tested on an Arduino Micro.

## Gameplay
Press any key to start the game. A countdown will appear on the 7-segment display, and then the game will start. Hit the key with the corresponding bar displayed above it. You have 1 minute to hit as many keys as possible. If you press an incorrect key the display will flash for a moment, and the time will continue to count down. You are unable to accrue more points while the display flashes.

The 7-segment display shows your score (# of keys pressed). When the time is up, your score will flash. The display will show "High Score!" if you placed in the top three high scores. You will then be able to enter your initials using the left 3 keys to cycle through the alphabet for 3 characters. Once your initials are input, press the rightmost key to submit your initials.

When idle, the display will cycle through the top 3 high scores and their corresponding initials. To reset all of the high scores you can press and hold all 4 keys for at least 15 seconds while the display is cycling through the high scores. The display will show "Clear?" after the keys have been held for 10 seconds and then "Clear!" after 5 more seconds.

## Dependencies
1. [Adafruit LED Backpack Library by Adafruit](https://github.com/adafruit/Adafruit_LED_Backpack)
2. [MD_MAX72XX Library by majicDesigns](https://github.com/MajicDesigns/MD_MAX72XX)

Both of these libraries are available for download through the Arduino IDE Library Manager.

## Wiring

## Materials
1. [Kailh Mechanical Key Switches](https://www.adafruit.com/product/4955)
2. [32x8 LED Matrix Display Module](https://www.aliexpress.us/item/2255800945325764.html?spm=a2g0o.detail.1000014.1.7eb9b097AdLYtk&gps-id=pcDetailBottomMoreOtherSeller&scm=1007.40000.317745.0&scm_id=1007.40000.317745.0&scm-url=1007.40000.317745.0&pvid=42198de2-00b1-4cb9-b964-f6308df7764c&_t=gps-id:pcDetailBottomMoreOtherSeller,scm-url:1007.40000.317745.0,pvid:42198de2-00b1-4cb9-b964-f6308df7764c,tpp_buckets:668%232846%238115%23875&pdp_ext_f=%7B%22sku_id%22%3A%2212000028237000958%22%2C%22sceneId%22%3A%2230050%22%7D&pdp_npi=2%40dis%21USD%211.06%210.9%21%21%21%21%21%40210312ee16727133512697117eeb00%2112000028237000958%21rec)
3. [0.56" 4-Digit 7-Segment Display](https://www.adafruit.com/product/881)
4. [Arduino Micro](https://store-usa.arduino.cc/products/arduino-micro?selectedStore=us)
5. M2 screws and nuts
6. [3D-Printed Parts](https://www.thingiverse.com/thing:5761761)
