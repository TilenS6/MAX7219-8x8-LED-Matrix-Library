/*
- Author: Tilen Stermecki
- Date: September 2024
- Made for use with Ardunio-type uC and MAX7219 chip with 8x8 single-color LED matrix
- Released into the public domain
*/

#include <Arduino.h>
#include "max7219/maxMatrix.h"
// uncomment this to save memory but disables drawCharacterImage(...) and slideText(...) functions
// #define DONT_INCLUDE_FONT

MaxMatrix matrix;

void setup() {
    matrix.init(A2, A1, A0); // CS, DIN, CLK: they could be any pin you want
}

void testDrawPixel() {
    matrix.clear();

    matrix.drawPixel(0, 0, MAX_DRAW); // draw pixel to each corner (0,0 = top-left)
    matrix.drawPixel(7, 0, MAX_DRAW);
    matrix.drawPixel(7, 7, MAX_DRAW);
    matrix.drawPixel(0, 7, MAX_DRAW);

    matrix.updateScreen(); // don't forget to upload data to the screen, your changes applies here

    delay(1000);
}

void testSquare() {
    matrix.clear();

    matrix.drawSquare(1, 1, 6, 6, MAX_DRAW); // hollow square
    matrix.updateScreen();
    delay(1000);

    matrix.fillSquare(1, 1, 6, 6, MAX_DRAW); // filled square
    matrix.updateScreen();
    delay(1000);
}

// website to construct your image (not sponsored, not mine): https://gurgleapps.com/tools/matrix
uint8_t smiley[8] = {0x3c, 0x42, 0xa5, 0x81, 0xa5, 0x99, 0x42, 0x3c};
void testImage() {
    for (int i = -7; i < 8; ++i) {
        matrix.clear();
        matrix.drawImage(i, 0, smiley, MAX_DrawImage_OVERWRITE); // TODO (fix comment) images are always 8x8 in size
        matrix.updateScreen();
        delay(100);
    }
}

uint8_t H_image[8] = {0xff, 0xa5, 0xa5, 0xbd, 0xa5, 0xa5, 0xa5, 0xff};
uint8_t L_image[8] = {0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x00};
void testMisc() {
    matrix.clear();
    matrix.drawImage(0, 0, H_image, MAX_DrawImage_OVERWRITE);
    matrix.drawSquare(0, 0, 8, 8, MAX_DRAW);
    matrix.updateScreen();
    matrix.intensity(15); // min brightness = 0 ; max brightness = 15, NOTE: this doesn't need update f. to apply changes
    delay(1000);

    matrix.clear();
    matrix.drawImage(0, 0, L_image, MAX_DrawImage_OVERWRITE);
    matrix.drawSquare(0, 0, 8, 8, MAX_DRAW);
    matrix.updateScreen();
    matrix.intensity(0);
    delay(1000);

    matrix.shutdown(true); // turns the screen off, nice way for blanking the screen temporarely
    delay(500);
    matrix.shutdown(false);
}

void testChars() {
    String message = "Hello world! -_%&#*/()[]{}";
    for (int i = -7; i < (int)message.length() * 8; i++) {
        matrix.clear();
        matrix.slideText(message, i);
        matrix.updateScreen();
        delay(50);
    }
}

void testAll() {
    testDrawPixel();
    testSquare();
    testImage();
    testMisc();
    testChars();

    delay(1000);
    matrix.clear();
    matrix.updateScreen();
    delay(5000);
}

// -------------------- MAIN -----------------------

void loop() {
    testAll();
}