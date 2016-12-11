/*
 * u8glib screens by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Shea Ivey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*
    The u8glib screens were started and later abandoned due to performance issues.
    Here for reference should anyone want to take on this task.

    Shea Ivey
*/

#include "settings.h"

#ifdef OLED_128x64_U8G_SCREENS
#include "screens.h" // function headers
#include <U8glib.h>

#define BLACK 0
#define WHITE 1


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI

screens::screens() {
    reset();
}


char screens::begin(const char *call_sign) {
    return 0;
}


void screens::reset() {
    u8g.setFont(u8g_font_fixed_v0);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
    u8g.setColorIndex(WHITE);
}

void screens::flip() {
    u8g.setRot180();
}

void screens::drawTitleBox(const char *title) {
    u8g.setColorIndex(WHITE);
    u8g.drawFrame(0, 0, u8g.getWidth(), u8g.getHeight());
    u8g.drawBox(0, 0, u8g.getWidth(), 11);
    u8g.setColorIndex(BLACK);

    // center text
    u8g.setPrintPos(((u8g.getWidth() - u8g.getStrWidth(title)) / 2),1);
    u8g.print(title);
}


void screens::mainMenu(uint8_t menu_id){
    u8g.firstPage();
    do {
        reset(); // start from fresh screen.
        drawTitleBox("MODE SELECTION");
        u8g.setColorIndex(WHITE);
        u8g.drawBox(0, 10*menu_id+12, u8g.getWidth(), 10);

        u8g.setColorIndex(menu_id == 0 ? BLACK : WHITE);
        u8g.setPrintPos(5,10*0+12);
        u8g.print("AUTO SEARCH");
        u8g.setColorIndex(menu_id == 1 ? BLACK : WHITE);
        u8g.setPrintPos(5,10*1+12);
        u8g.print("BAND SCANNER");
        u8g.setColorIndex(menu_id == 2 ? BLACK : WHITE);
        u8g.setPrintPos(5,10*2+12);
        u8g.print("MANUAL MODE");

#ifdef USE_DIVERSITY
        u8g.setColorIndex(menu_id == 3 ? BLACK : WHITE);
        u8g.setPrintPos(5,10*3+12);
        u8g.print("DIVERSITY");
#endif
        u8g.setColorIndex(menu_id == 4 ? BLACK : WHITE);
        u8g.setPrintPos(5,10*4+12);
        u8g.print("SAVE SETUP");
    } while( u8g.nextPage() );
}

void screens::seekMode(uint8_t state) {
    last_channel = -1;
    reset(); // start from fresh screen.
}

void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, uint8_t rssi_seek_threshold, bool locked) {

    last_channel = channel;
}

void screens::bandScanMode(uint8_t state) {
    reset(); // start from fresh screen.
    best_rssi = 0;
    if(state==STATE_SCAN)
    {
        drawTitleBox("BAND SCANNER");
    }
    else
    {
        drawTitleBox("RSSI SETUP");
    }
}

void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {
    #define SCANNER_LIST_X_POS 60
    static uint8_t writePos=SCANNER_LIST_X_POS;
    last_channel = channel;
}

void screens::screenSaver(uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    screenSaver(-1, channelName, channelFrequency, call_sign);
}
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {

}

void screens::updateScreenSaver(uint8_t rssi) {
    updateScreenSaver(-1, rssi, -1, -1);
}
void screens::updateScreenSaver(char active_receiver, uint8_t rssi, uint8_t rssiA, uint8_t rssiB) {

}

#ifdef USE_DIVERSITY
void screens::diversity(uint8_t diversity_mode) {
    reset();
    drawTitleBox("DIVERSITY");
}
void screens::updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB){

}
#endif
void screens::setupMenu(){
}

void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency,const char *call_sign) {
    reset();
    drawTitleBox("SAVE SETTINGS");
}

void screens::updateSave(const char * msg) {
}
#endif
