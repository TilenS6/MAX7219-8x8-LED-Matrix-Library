/*
- Author: Tilen Stermecki
- Date: September 2024
- Made for use with Ardunio-type uC and MAX7219 chip with 8x8 single-color LED matrix
- Released into the public domain

- * Documentation for MAX7219 is publicly avaliable here: https://www.analog.com/media/en/technical-documentation/data-sheets/MAX7219-MAX7221.pdf
*/

#ifndef MAXMATRIX
#define MAXMATRIX

#include <Arduino.h>

#define MAX_CLEAR 0
#define MAX_DRAW 1
#define MAX_FLIP 2

#define MAX_DrawImage_CLEAR 0
#define MAX_DrawImage_DRAW 3
#define MAX_DrawImage_FLIP 2
#define MAX_DrawImage_OVERWRITE 1

#define MAX7219OP_No_Op 0x00
#define MAX7219OP_Digit_0 0x01
#define MAX7219OP_Digit_1 0x02
#define MAX7219OP_Digit_2 0x03
#define MAX7219OP_Digit_3 0x04
#define MAX7219OP_Digit_4 0x05
#define MAX7219OP_Digit_5 0x06
#define MAX7219OP_Digit_6 0x07
#define MAX7219OP_Digit_7 0x08
#define MAX7219OP_Decode_Mode 0x09
#define MAX7219OP_Intensity 0x0A
#define MAX7219OP_Scan_Limit 0x0B
#define MAX7219OP_Shutdown 0x0C
#define MAX7219OP_Display_Test 0x0F

class MaxMatrix {
    bool first = true;
    int cs, din, clk;
    uint8_t data[8];
    uint8_t changed; // for partial updating
    void writeCommand(uint8_t, uint8_t);
    void drawPixel_noCheck(uint8_t, uint8_t, uint8_t);

public:
    void init(int, int, int);
    void reinit(); // use reinit regularly to ensure connection doesn't drop in high EMF areas (also great for hot-pluging?)

    void clear();
    void drawPixel(uint8_t, uint8_t, uint8_t);
    void drawSquare(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
    void fillSquare(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
    void drawImage(int8_t, int8_t, uint8_t[8], uint8_t);
    void drawCharacterImage(char, int8_t, int8_t, uint8_t);
    void slideText(String, int);

    void updateScreen();

    // display controll
    void shutdown(bool);
    void intensity(uint8_t);
    void displayTest(bool);
};

#include "maxMatrix_font.cpp"
#include "maxMatrix.cpp"

#endif