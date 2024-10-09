#include "maxMatrix.h"

void MaxMatrix::init(int CS, int DIN, int CLK) {
    cs = CS;
    din = DIN;
    clk = CLK;

    clear();
    reinit();
}

void MaxMatrix::reinit() {
    pinMode(cs, OUTPUT);
    pinMode(din, OUTPUT);
    pinMode(clk, OUTPUT);
    digitalWrite(cs, 1);
    digitalWrite(din, 0);
    digitalWrite(clk, 0);
    changed = 255;

    if (first) {
        writeCommand(MAX7219OP_Shutdown, 0x00); // shutdown the display
        first = false;
    }

    writeCommand(MAX7219OP_Display_Test, 0x00); // don't test
    writeCommand(MAX7219OP_Intensity, 0x00);    // (min=0x00, max=0x0F)
    writeCommand(MAX7219OP_Decode_Mode, 0x00);  // bit-bit decoding (not BCD)
    writeCommand(MAX7219OP_Scan_Limit, 0x07);   // scan all

    updateScreen();

    writeCommand(MAX7219OP_Shutdown, 0x01); // normal operation
}

void MaxMatrix::writeCommand(uint8_t address, uint8_t data) {
    uint16_t chunk = ((((uint16_t)address) << 8) | data) & 0x0FFF;

    // it should work delay-less. All timings are <=100ns
    digitalWrite(cs, 0);
    for (int8_t i = 15; i >= 0; --i) {
        digitalWrite(din, (bool)(chunk & (1 << i))); // read bit by bit
        digitalWrite(clk, 1);
        digitalWrite(clk, 0);
    }
    digitalWrite(cs, 1);
    digitalWrite(din, 0);
}

void MaxMatrix::clear() {
    *((uint64_t *)data) = 0;
    changed = 255;
}

void MaxMatrix::updateScreen() {
    for (int i = 0; i < 8; ++i) {
        if (changed & (1 << i))
            writeCommand(MAX7219OP_Digit_0 + i, data[i]);
    }
    changed = 0;
}

void MaxMatrix::drawPixel_noCheck(uint8_t x, uint8_t y, uint8_t val) {
    changed |= (1 << x);
    switch (val) {
    case MAX_DRAW:
        data[x] |= (1 << y);
        break;
    case MAX_CLEAR:
        data[x] &= ~(1 << y);
        break;
    case MAX_FLIP:
        data[x] ^= (1 << y);
        break;
    default:
        break;
    }
}

void MaxMatrix::drawPixel(uint8_t x, uint8_t y, uint8_t val = MAX_DRAW) {
    if (x >= 8 || y >= 8) return;
    drawPixel_noCheck(x, y, val);
}

void MaxMatrix::drawSquare(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t val = MAX_DRAW) {
    uint8_t borderY = y + h - 1;
    uint8_t borderX = x + w - 1;
    for (int i = 0; i < w; ++i) {
        uint8_t xx = x + i;
        if (xx >= 8) break;
        drawPixel(xx, y, val);
        drawPixel(xx, borderY, val);
    }
    for (int i = 1; i < h - 1; ++i) {
        uint8_t yy = y + i;
        if (yy >= 8) break;
        drawPixel(x, yy, val);
        drawPixel(borderX, yy, val);
    }
}

void MaxMatrix::fillSquare(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t val = MAX_DRAW) {
    for (int i = 0; i < h; ++i) {
        if (y + i >= 8) break;
        for (int ii = 0; ii < w; ++ii) {
            if (x + ii >= 8) break;
            drawPixel(x + ii, y + i, val);
        }
    }
}

void MaxMatrix::drawImage(int8_t x, int8_t y, uint8_t buff_in[8], uint8_t type = MAX_DrawImage_OVERWRITE) {
    uint8_t buff[8];
    for (int i = 0; i < 8; ++i)
        buff[i] = 0;

    // rotate data matrix for 90deg (flip x-y)
    for (int yy = 0; yy < 8; ++yy) {
        uint8_t chunk = 0;
        for (int xx = 0; xx < 8; ++xx) {
            if (buff_in[xx] & (1 << yy)) {
                chunk |= (1 << xx);
            }
        }
        buff[7 - yy] = chunk;
    }
    /*
    for (int i = 0; i < 8; ++i) {
        data[i] = buff[i];
    }
    return;
    */
    for (uint8_t xx = 0; xx < 8; xx++) {
        if (x + xx >= 8) break;
        if (x + xx < 0) continue;

        uint8_t workBuff = 0;
        if (y >= 0) {
            workBuff = buff[xx] << y;
        } else {
            workBuff = buff[xx] >> (-y);
        }

        switch (type) {
        case MAX_DrawImage_CLEAR:
            data[x + xx] &= ~workBuff; // flipped bits
            break;
        case MAX_DrawImage_DRAW:
            data[x + xx] |= workBuff;
            break;
        case MAX_DrawImage_FLIP:
            data[x + xx] ^= workBuff;
            break;
        case MAX_DrawImage_OVERWRITE:
            uint8_t mask = 0xFF;
            if (y >= 0) {
                mask = ~(255 << y);
            } else {
                mask = ~(255 >> (-y));
            }
            data[x + xx] &= mask;     // clear spots
            data[x + xx] |= workBuff; // set data
            break;
        }
    }

    if (x >= 0) {
        changed |= 255 << x;
    } else {
        changed |= 255 >> (-x);
    }
}

void MaxMatrix::drawCharacterImage(char c, int8_t x, int8_t y, uint8_t type = MAX_DrawImage_OVERWRITE) {
#ifdef DONT_INCLUDE_FONT
    return;
#endif

    if (c < 0 || c >= 128) return;
    drawImage(x, y, font[(uint8_t)c], type);
}

void MaxMatrix::slideText(String text, int offset) {
#ifdef DONT_INCLUDE_FONT
    return;
#endif

    int i = offset / 8; // which character to show first
    int rel_offset = offset % 8;
    if (i >= 0 && i < (int)text.length()) {
        drawCharacterImage(text[i], -rel_offset, 0);
    }

    if (i >= -1 && (i + 1) < (int)text.length()) { // show next one too
        drawCharacterImage(text[i + 1], -rel_offset + 8, 0);
    }
}

void MaxMatrix::shutdown(bool state) {
    writeCommand(MAX7219OP_Shutdown, (!state));
}

void MaxMatrix::intensity(uint8_t val) { // val: [0, 0x0F=15]
    if (val > 0x0F) val = 0x0F;

    writeCommand(MAX7219OP_Intensity, val);
}

void MaxMatrix::displayTest(bool state) {
    writeCommand(MAX7219OP_Display_Test, (!state));
}