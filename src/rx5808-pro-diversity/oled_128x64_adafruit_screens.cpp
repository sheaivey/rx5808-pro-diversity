/*
 * OLED Screens by Shea Ivey

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

#include "settings.h"

#ifdef OLED_128x64_ADAFRUIT_SCREENS
#include "screens.h" // function headers
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

// New version of PSTR that uses a temp buffer and returns char *
// by Shea Ivey
#define PSTR2(x) PSTRtoBuffer_P(PSTR(x))
char PSTR2_BUFFER[30]; // adjust size depending on need.
char *PSTRtoBuffer_P(PGM_P str) { uint8_t c='\0', i=0; for(; (c = pgm_read_byte(str)) && i < sizeof(PSTR2_BUFFER); str++, i++) PSTR2_BUFFER[i]=c;PSTR2_BUFFER[i]=c; return PSTR2_BUFFER;}

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if !defined SSD1306_128_64
    #error("Screen size incorrect, please fix Adafruit_SSD1306.h!");
#endif

screens::screens() {
    last_channel = -1;
    last_rssi = 0;
}

char screens::begin() {
    // Set the address of your OLED Display.
    // 128x64 ONLY!!
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
    // init done
    reset();

    display.fillRect(0, 0, display.width(), 11,WHITE);
    display.setTextColor(BLACK);
    display.setCursor(((display.width() - (strlen(PSTR2(CALL_SIGN))*6)) / 2),2);
    display.print(PSTR2("Boot Check"));

    display.setTextColor(WHITE);
    display.setCursor(0,8*1+4);
    display.print(PSTR2("Power:"));
    display.setCursor(display.width()-6*2,8*1+4);
    display.print(PSTR2("OK"));
    display.setCursor(0,8*2+4);

    display.display();
#ifdef USE_DIVERSITY
    display.print(PSTR2("Diversity:"));
    display.display();
    delay(500);
    display.setCursor(display.width()-6*8,8*2+4);
    if(isDiversity()) {
        display.print(PSTR2(" ENABLED"));
    }
    else {
        display.print(PSTR2("DISABLED"));
    }
#endif
    display.setCursor(((display.width() - (strlen(PSTR2(CALL_SIGN))*13)) / 2),8*4+4);
    display.setTextSize(2);
    display.print(PSTR2(CALL_SIGN));
    display.display();
    delay(2000);
    return 0; // no errors
}

void screens::reset() {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void screens::flip() {
    display.setRotation(3);
}

void screens::drawTitleBox(const char *title) {
    display.drawRect(0, 0, display.width(), display.height(),WHITE);
    display.fillRect(0, 0, display.width(), 11,WHITE);

    display.setTextSize(1);
    display.setTextColor(BLACK);
    // center text
    display.setCursor(((display.width() - (strlen(title)*6)) / 2),2);
    display.print(title);
    display.setTextColor(WHITE);
}

void screens::mainMenu(uint8_t menu_id) {
    reset(); // start from fresh screen.
    drawTitleBox(PSTR2("MODE SELECTION"));

    display.fillRect(0, 10*menu_id+12, display.width(), 10,WHITE);

    display.setTextColor(menu_id == 0 ? BLACK : WHITE);
    display.setCursor(5,10*0+13);
    display.print(PSTR2("AUTO SEARCH"));
    display.setTextColor(menu_id == 1 ? BLACK : WHITE);
    display.setCursor(5,10*1+13);
    display.print(PSTR2("BAND SCANNER"));
    display.setTextColor(menu_id == 2 ? BLACK : WHITE);
    display.setCursor(5,10*2+13);
    display.print(PSTR2("MANUAL MODE"));

#ifdef USE_DIVERSITY
    if(isDiversity())
    {
        display.setTextColor(menu_id == 3 ? BLACK : WHITE);
        display.setCursor(5,10*3+13);
        display.print(PSTR2("DIVERSITY"));
    }
#endif
    display.setTextColor(menu_id == 4 ? BLACK : WHITE);
    display.setCursor(5,10*4+13);
    display.print(PSTR2("SAVE SETUP"));

    display.display();
}

void screens::seekMode(uint8_t state) {
    last_channel = -1;
    reset(); // start from fresh screen.
    if (state == STATE_MANUAL)
    {
        drawTitleBox(PSTR2("MANUAL MODE"));
    }
    else if(state == STATE_SEEK)
    {
        drawTitleBox(PSTR2("AUTO SEEK MODE"));
    }
    display.setTextColor(WHITE);
    display.drawLine(0, 20, display.width(), 20, WHITE);
    display.drawLine(0, 32, display.width(), 32, WHITE);
    display.setCursor(5,12);
    display.drawLine(97,11,97,20,WHITE);
    display.print(PSTR2("BAND:"));
    for(uint16_t i=0;i<8;i++) {
        display.setCursor(15*i+8,23);
        display.print((char) (i+'1'));
    }
    display.drawLine(0, 36, display.width(), 36, WHITE);
    display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    display.setCursor(2,display.height()-9);
    display.print(PSTR2("5645"));
    display.setCursor(55,display.height()-9);
    display.print(PSTR2("5800"));
    display.setCursor(display.width()-25,display.height()-9);
    display.print(PSTR2("5945"));
    display.display();
}

void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, bool locked) {
    // display refresh handler
    if(channelIndex != last_channel) // only updated on changes
    {
        display.setTextColor(WHITE,BLACK);
        display.setCursor(36,12);
        // show current used channel of bank
        if(channelIndex > 31)
        {
            display.print(PSTR2("C/Race   "));
        }
        else if(channelIndex > 23)
        {
            display.print(PSTR2("F/Airwave"));
        }
        else if (channelIndex > 15)
        {
            display.print(PSTR2("E        "));
        }
        else if (channelIndex > 7)
        {
            display.print(PSTR2("B        "));
        }
        else
        {
            display.print(PSTR2("A        "));
        }

        uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE; // get channel inside band
        for(int i=0;i<8;i++) {
            display.fillRect(15*i+4,21,14,11,i==active_channel? WHITE:BLACK);
            display.setTextColor(i==active_channel? BLACK:WHITE);
            display.setCursor(15*i+8,23);
            display.print((char) (i+'1'));
        }

        // show frequence
        display.setCursor(101,12);
        display.setTextColor(WHITE,BLACK);
        display.print(channelFrequency);
    }
    // show signal strength
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, display.width()-3);

    display.fillRect(1+rssi_scaled, 33, display.width()-3-rssi_scaled, 3, BLACK);
    display.fillRect(1, 33, rssi_scaled, 3, WHITE);

    rssi_scaled=map(rssi, 1, 100, 1, 14);
    display.fillRect((channel*3)+4,display.height()-12-14,3,14-rssi_scaled,BLACK);
    display.fillRect((channel*3)+4,(display.height()-12-rssi_scaled),3,rssi_scaled,WHITE);

    // handling for seek mode after screen and RSSI has been fully processed
    if(state == STATE_SEEK) //
    { // SEEK MODE
        if(locked) // search if not found
        {
            display.setTextColor(BLACK,WHITE);
            display.setCursor(((display.width()-14*6)/2),2);
            display.print(PSTR2("AUTO MODE LOCK"));
        }
        else
        {
            display.setTextColor(BLACK,WHITE);
            display.setCursor(((display.width()-14*6)/2),2);
            display.print(PSTR2("AUTO SEEK MODE"));
        }
    }

    last_channel = channel;
    display.display();
}

void screens::bandScanMode(uint8_t state) {
    reset(); // start from fresh screen.
    best_rssi = 0;
    if(state==STATE_SCAN)
    {
        drawTitleBox(PSTR2("BAND SCANNER"));
        display.setCursor(5,12);
        display.print(PSTR2("BEST:"));
    }
    else
    {
        drawTitleBox(PSTR2("RSSI SETUP"));
        display.setCursor(5,12);
        display.print(PSTR2("Min:     Max:"));
    }
    display.drawLine(0, 20, display.width(), 20, WHITE);

    display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    display.setCursor(2,display.height()-9);
    display.print(PSTR2("5645"));
    display.setCursor(55,display.height()-9);
    display.print(PSTR2("5800"));
    display.setCursor(display.width()-25,display.height()-9);
    display.print(PSTR2("5945"));
    display.display();
}

void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {
    #define SCANNER_LIST_X_POS 60
    static uint8_t writePos = SCANNER_LIST_X_POS;
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, 30);
    uint16_t hight = (display.height()-12-rssi_scaled);
    if(channel != last_channel) // only updated on changes
    {
        display.fillRect((channel*3)+4,display.height()-12-30,3,30-rssi_scaled,BLACK);
        display.fillRect((channel*3)+4,hight,3,rssi_scaled,WHITE);
    }
    if(!in_setup) {
        if (rssi > RSSI_SEEK_TRESHOLD) {
            if(best_rssi < rssi) {
                best_rssi = rssi;
                display.setTextColor(WHITE,BLACK);
                display.setCursor(36,12);
                display.print(channelName, HEX);
                display.setCursor(52,12);
                display.print(channelFrequency);
            }
            else {
                if(writePos+10>display.width()-12)
                { // keep writing on the screen
                    writePos=SCANNER_LIST_X_POS;
                }
            }
        }
    }
    else {
        display.setCursor(30,12);
        display.setTextColor(WHITE,BLACK);
        display.print( PSTR2("   ") );
        display.setCursor(30,12);
        display.print( rssi_setup_min_a , DEC);

        display.setCursor(85,12);
        display.setTextColor(WHITE,BLACK);
        display.setCursor(85,12);
        display.print( PSTR2("   ") );
        display.print( rssi_setup_max_a , DEC);
    }
    display.display();
    last_channel = channel;
}

void screens::screenSaver(uint8_t channelName, uint16_t channelFrequency) {
    screenSaver(-1, channelName, channelFrequency);
}
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency) {
    reset();
    display.setTextSize(6);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(channelName, HEX);
    display.setTextSize(1);
    display.setCursor(70,0);
    display.print(PSTR2(CALL_SIGN));
    display.setTextSize(2);
    display.setCursor(70,28);
    display.setTextColor(WHITE);
    display.print(channelFrequency);
    display.setTextSize(1);
#ifdef USE_DIVERSITY
    if(isDiversity()) {
        display.setCursor(70,18);
        switch(diversity_mode) {
            case useReceiverAuto:
                display.print(PSTR2("AUTO"));
                break;
            case useReceiverA:
                display.print(PSTR2("ANTENNA A"));
                break;
            case useReceiverB:
                display.print(PSTR2("ANTENNA B"));
                break;
        }
        display.setTextColor(BLACK,WHITE);
        display.fillRect(0, display.height()-19, 7, 9, WHITE);
        display.setCursor(1,display.height()-18);
        display.print("A");
        display.setTextColor(BLACK,WHITE);
        display.fillRect(0, display.height()-9, 7, 9, WHITE);
        display.setCursor(1,display.height()-8);
        display.print("B");
    }
#endif
    display.display();
}

void screens::updateScreenSaver(uint8_t rssi) {
    updateScreenSaver(-1, rssi, -1, -1);
}
void screens::updateScreenSaver(char active_receiver, uint8_t rssi, uint8_t rssiA, uint8_t rssiB) {
#ifdef USE_DIVERSITY
    if(isDiversity()) {
        // read rssi A
        #define RSSI_BAR_SIZE 119
        uint8_t rssi_scaled=map(rssiA, 1, 100, 3, RSSI_BAR_SIZE);
        display.fillRect(7 + rssi_scaled, display.height()-19, (RSSI_BAR_SIZE-rssi_scaled), 9, BLACK);
        if(active_receiver == useReceiverA)
        {
            display.fillRect(7, display.height()-19, rssi_scaled, 9, WHITE);
        }
        else
        {
            display.fillRect(7, display.height()-19, (RSSI_BAR_SIZE), 9, BLACK);
            display.drawRect(7, display.height()-19, rssi_scaled, 9, WHITE);
        }

        // read rssi B
        rssi_scaled=map(rssiB, 1, 100, 3, RSSI_BAR_SIZE);
        display.fillRect(7 + rssi_scaled, display.height()-9, (RSSI_BAR_SIZE-rssi_scaled), 9, BLACK);
        if(active_receiver == useReceiverB)
        {
            display.fillRect(7, display.height()-9, rssi_scaled, 9, WHITE);
        }
        else
        {
            display.fillRect(7, display.height()-9, (RSSI_BAR_SIZE), 9, BLACK);
            display.drawRect(7, display.height()-9, rssi_scaled, 9, WHITE);
        }
    }
    else {
        display.setTextColor(BLACK);
        display.fillRect(0, display.height()-19, 25, 19, WHITE);
        display.setCursor(1,display.height()-13);
        display.print(PSTR2("RSSI"));
        #define RSSI_BAR_SIZE 101
        uint8_t rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
        display.fillRect(25 + rssi_scaled, display.height()-19, (RSSI_BAR_SIZE-rssi_scaled), 19, BLACK);
        display.fillRect(25, display.height()-19, rssi_scaled, 19, WHITE);
    }
#else
    display.setTextColor(BLACK);
    display.fillRect(0, display.height()-19, 25, 19, WHITE);
    display.setCursor(1,display.height()-13);
    display.print(PSTR2("RSSI"));
    #define RSSI_BAR_SIZE 101
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
    display.fillRect(25 + rssi_scaled, display.height()-19, (RSSI_BAR_SIZE-rssi_scaled), 19, BLACK);
    display.fillRect(25, display.height()-19, rssi_scaled, 19, WHITE);
#endif
    if(rssi < 20)
    {
        display.setTextColor((millis()%250 < 125) ? WHITE : BLACK, BLACK);
        display.setCursor(50,display.height()-13);
        display.print(PSTR2("LOW SIGNAL"));
    }
#ifdef USE_DIVERSITY
    else if(isDiversity()) {
        display.drawLine(50,display.height()-10,110,display.height()-10,BLACK);
    }
#endif
    display.display();
}

#ifdef USE_DIVERSITY
void screens::diversity(uint8_t diversity_mode) {

    reset();
    drawTitleBox(PSTR2("DIVERSITY"));

    //selected
    display.fillRect(0, 10*diversity_mode+12, display.width(), 10, WHITE);

    display.setTextColor(diversity_mode == useReceiverAuto ? BLACK : WHITE);
    display.setCursor(5,10*1+3);
    display.print(PSTR2("AUTO"));

    display.setTextColor(diversity_mode == useReceiverA ? BLACK : WHITE);
    display.setCursor(5,10*2+3);
    display.print(PSTR2("RECEIVER A"));
    display.setTextColor(diversity_mode == useReceiverB ? BLACK : WHITE);
    display.setCursor(5,10*3+3);
    display.print(PSTR2("RECEIVER B"));

    // RSSI Strength
    display.setTextColor(WHITE);
    display.drawRect(0, display.height()-21, display.width(), 11, WHITE);
    display.setCursor(5,display.height()-19);
    display.print("A:");
    display.setCursor(5,display.height()-9);
    display.print("B:");
    display.display();
}

void screens::updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB){
    #define RSSI_BAR_SIZE 108
    uint8_t rssi_scaled=map(rssiA, 1, 100, 1, RSSI_BAR_SIZE);

    display.fillRect(18 + rssi_scaled, display.height()-19, (RSSI_BAR_SIZE-rssi_scaled), 7, BLACK);
    if(active_receiver==useReceiverA)
    {
        display.fillRect(18, display.height()-19, rssi_scaled, 7, WHITE);
    }
    else
    {
        display.fillRect(18, display.height()-19, rssi_scaled, 7, BLACK);
        display.drawRect(18, display.height()-19, rssi_scaled, 7, WHITE);
    }

    // read rssi B
    rssi_scaled=map(rssiB, 1, 100, 1, RSSI_BAR_SIZE);
    display.fillRect(18 + rssi_scaled, display.height()-9, (RSSI_BAR_SIZE-rssi_scaled), 7, BLACK);
    if(active_receiver==useReceiverB)
    {
        display.fillRect(18, display.height()-9, rssi_scaled, 7, WHITE);
    }
    else
    {
        display.fillRect(18, display.height()-9, rssi_scaled, 7, BLACK);
        display.drawRect(18, display.height()-9, rssi_scaled, 7, WHITE);
    }
    display.display();
}
#endif

void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency) {
    reset();
    drawTitleBox(PSTR2("SAVE SETTINGS"));

    display.setTextColor(WHITE);
    display.setCursor(5,8*1+4);
    display.print(PSTR2("MODE:"));
    display.setCursor(38,8*1+4);
    switch (mode)
    {
        case STATE_SCAN: // Band Scanner
            display.print(PSTR2("BAND SCANNER"));
        break;
        case STATE_MANUAL: // manual mode
            display.print(PSTR2("MANUAL"));
        break;
        case STATE_SEEK: // seek mode
            display.print(PSTR2("AUTO SEEK"));
        break;
    }

    display.setCursor(5,8*2+4);
    display.print(PSTR2("BAND:"));
    display.setCursor(38,8*2+4);
    // print band
    if(channelIndex > 31)
    {
        display.print(PSTR2("C/Race"));
    }
    else if(channelIndex > 23)
    {
        display.print(PSTR2("F/Airwave"));
    }
    else if (channelIndex > 15)
    {
        display.print(PSTR2("E"));
    }
    else if (channelIndex > 7)
    {
        display.print(PSTR2("B"));
    }
    else
    {
        display.print(PSTR2("A"));
    }

    display.setCursor(5,8*3+4);
    display.print(PSTR2("CHAN:"));
    display.setCursor(38,8*3+4);
    uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE+1; // get channel inside band
    display.print(active_channel,DEC);
    display.setCursor(5,8*4+4);
    display.print(PSTR2("FREQ:     GHz"));
    display.setCursor(38,8*4+4);
    display.print(channelFrequency);
    display.setCursor(((display.width()-11*6)/2),8*5+4);
    display.print(PSTR2("-- SAVED --"));
    display.display();
}

void screens::updateSave(const char * msg) {
    display.setTextColor(WHITE,BLACK);
    display.setCursor(((display.width()-strlen(msg)*6)/2),8*6+4);
    display.print(msg);
    display.display();
}


#endif
