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

void loop() {
}