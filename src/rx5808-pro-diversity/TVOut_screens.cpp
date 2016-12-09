/*
 * TVOUT by Myles Metzel
 * Refactored and GUI reworked by Marko Hoepken
 * Universal version my Marko Hoepken
 * Diversity Receiver Mode and GUI improvements by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Marko Hoepken

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

#ifdef TVOUT_SCREENS
#include "screens.h" // function headers
#include <Arduino.h>


#include <TVout.h>
#include <fontALL.h>

// Set you TV format (PAL = Europe = 50Hz, NTSC = INT = 60Hz)
//#define TV_FORMAT NTSC
#define TV_FORMAT PAL

#define TV_COLS 128
#define TV_ROWS 96
#define TV_Y_MAX TV_ROWS-1
#define TV_X_MAX TV_COLS-1
#define TV_SCANNER_OFFSET 14
#define SCANNER_BAR_SIZE 52
#define SCANNER_LIST_X_POS 54
#define SCANNER_LIST_Y_POS 16
#define SCANNER_MARKER_SIZE 1

#ifdef USE_VOLTAGE_MONITORING
    #define MENU_Y_SIZE 13
#else
    #define MENU_Y_SIZE 15
#endif
#define TV_Y_GRID 14
#define TV_Y_OFFSET 3


TVout TV;

screens::screens() {
    last_channel = -1;
    last_rssi = 0;
}

char screens::begin(const char *call_sign) {
    // 0 if no error.
    // 1 if x is not divisable by 8.
    // 2 if y is to large (NTSC only cannot fill PAL vertical resolution by 8bit limit)
    // 4 if there is not enough memory for the frame buffer.
    return TV.begin(TV_FORMAT, TV_COLS, TV_ROWS);
}

void screens::reset() {
    TV.clear_screen();
    TV.select_font(font8x8);
}

void screens::flip() {
}

void screens::drawTitleBox(const char *title) {
    TV.draw_rect(0,0,127,95,  WHITE);
    TV.printPGM(((127-strlen_P(title)*8)/2), 3,  title);
    TV.draw_rect(0,0,127,14,  WHITE,INVERT);
}
void screens::drawBottomTriangle(bool color){
    //isn't needed NOW for tvscreen
}
void screens::drawTopTriangle(bool color){
    //isn't needed NOW for tvscreen
}

void screens::mainMenu(uint8_t menu_id) {
    reset(); // start from fresh screen.
    drawTitleBox(PSTR("MODE SELECTION"));

    TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Auto Search"));
    TV.printPGM(10, 5+2*MENU_Y_SIZE, PSTR("Band Scanner"));
    TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("Manual Mode"));
#ifdef USE_DIVERSITY
    if( isDiversity() )
    {
        TV.printPGM(10, 5+4*MENU_Y_SIZE, PSTR("Diversity"));
    }
#endif
    TV.printPGM(10, 5+5*MENU_Y_SIZE, PSTR("Setup Menu"));
    // selection by inverted box

    TV.draw_rect(0,3+(menu_id+1)*MENU_Y_SIZE,127,12,  WHITE, INVERT);
}

void screens::seekMode(uint8_t state) {
    last_channel = -1;
    reset(); // start from fresh screen.
    if (state == STATE_MANUAL)
    {
        drawTitleBox(PSTR("MANUAL MODE"));
    }
    else if(state == STATE_SEEK)
    {
        drawTitleBox(PSTR("AUTO SEEK MODE"));
    }
    TV.draw_line(0,1*TV_Y_GRID,TV_X_MAX,1*TV_Y_GRID,WHITE);
    TV.printPGM(5,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("BAND: "));
    TV.draw_line(0,2*TV_Y_GRID,TV_X_MAX,2*TV_Y_GRID,WHITE);
    TV.printPGM(5 ,TV_Y_OFFSET-1+2*TV_Y_GRID,  PSTR("1 2 3 4 5 6 7 8"));
    TV.draw_line(0,3*TV_Y_GRID,TV_X_MAX,3*TV_Y_GRID,WHITE);
    TV.printPGM(5,TV_Y_OFFSET+3*TV_Y_GRID,  PSTR("FREQ:     GHz"));
    TV.draw_line(0,4*TV_Y_GRID,TV_X_MAX,4*TV_Y_GRID,WHITE);
    TV.select_font(font4x6);
    TV.printPGM(5,TV_Y_OFFSET+4*TV_Y_GRID,  PSTR("RSSI:"));
    TV.draw_line(0,5*TV_Y_GRID-4,TV_X_MAX,5*TV_Y_GRID-4,WHITE);
    // frame for tune graph
    TV.draw_rect(0,TV_ROWS - TV_SCANNER_OFFSET,TV_X_MAX,13,  WHITE); // lower frame
#ifdef USE_LBAND
    TV.print(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5362");
#else
    TV.print(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5645");
#endif
    TV.print(57, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5800");
    TV.print(111, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5945");

}

void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, uint8_t rssi_seek_threshold, bool locked) {
    // display refresh handler
    TV.select_font(font8x8);
    if(channelIndex != last_channel) // only updated on changes
    {
        // show current used channel of bank
#ifdef USE_LBAND
        if(channelIndex > 39)
        {
            TV.printPGM(50,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("D/5.3    "));
        }
        else if(channelIndex > 31)
#else
        if(channelIndex > 31)
#endif
        {
            TV.printPGM(50,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("C/Race   "));
        }
        else if(channelIndex > 23)
        {
            TV.printPGM(50,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("F/Airwave"));
        }
        else if (channelIndex > 15)
        {
            TV.printPGM(50,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("E        "));
        }
        else if (channelIndex > 7)
        {
            TV.printPGM(50,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("B        "));
        }
        else
        {
            TV.printPGM(50,TV_Y_OFFSET+1*TV_Y_GRID,  PSTR("A        "));
        }
        // show channel inside band
        uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE; // get channel inside band

        TV.draw_rect(1 ,TV_Y_OFFSET-2+2*TV_Y_GRID,125,12,  BLACK, BLACK); // mark current channel
        TV.printPGM(5 ,TV_Y_OFFSET-1+2*TV_Y_GRID,  PSTR("1 2 3 4 5 6 7 8"));
        // set new marker
        TV.draw_rect(active_channel*16+2 ,TV_Y_OFFSET-2+2*TV_Y_GRID,12,12,  WHITE, INVERT); // mark current channel

        // clear last square
        TV.draw_rect(1, (TV_ROWS - TV_SCANNER_OFFSET + 8),125,SCANNER_MARKER_SIZE,  BLACK, BLACK);
        // draw next
#ifdef USE_LBAND
        TV.draw_rect((channel * 5/2)+5, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);

#else
        TV.draw_rect((channel * 3)+5, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);

#endif
        // show frequence
        TV.print(50,TV_Y_OFFSET+3*TV_Y_GRID, channelFrequency);
    }
    // show signal strength
    #define RSSI_BAR_SIZE 100
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
    // clear last bar
    TV.draw_rect(25, TV_Y_OFFSET+4*TV_Y_GRID, RSSI_BAR_SIZE,4 , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect(25, TV_Y_OFFSET+4*TV_Y_GRID, rssi_scaled, 4 , WHITE, WHITE);
    // print bar for spectrum

    #define SCANNER_BAR_MINI_SIZE 14

    rssi_scaled=map(rssi, 1, 100, 1, SCANNER_BAR_MINI_SIZE);

 
#ifdef USE_LBAND
    // clear last bar
    TV.draw_rect((channel * 5/2)+4, (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_MINI_SIZE), 2, SCANNER_BAR_MINI_SIZE , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect((channel * 5/2)+4, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), 2, rssi_scaled , WHITE, WHITE);
#else
   // clear last bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_MINI_SIZE), 2, SCANNER_BAR_MINI_SIZE , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), 2, rssi_scaled , WHITE, WHITE);
#endif
    // handling for seek mode after screen and RSSI has been fully processed
    if(state == STATE_SEEK)
    { // SEEK MODE

        rssi_scaled=map(rssi_seek_threshold, 1, 100, 1, SCANNER_BAR_MINI_SIZE);

        TV.draw_rect(1,(TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_MINI_SIZE),2,SCANNER_BAR_MINI_SIZE-1,BLACK,BLACK);
        TV.draw_line(1,(TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled),3,(TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), WHITE);
        TV.draw_rect(TV_X_MAX-2,(TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_MINI_SIZE),1,SCANNER_BAR_MINI_SIZE-1,BLACK,BLACK);
        TV.draw_line(TV_X_MAX-2,(TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled),TV_X_MAX,(TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), WHITE);
        if(last_channel != channelIndex) {
            // fix title flicker
            TV.draw_rect(0,0,127,14, WHITE,BLACK);
            if(locked) {
                TV.printPGM(((127-14*8)/2), TV_Y_OFFSET,  PSTR("AUTO MODE LOCK"));
            }
            else
            {
                TV.printPGM(((127-14*8)/2), TV_Y_OFFSET,  PSTR("AUTO MODE SEEK"));
            }
            TV.draw_rect(0,0,127,14,  WHITE,INVERT);
        }
    }

    last_channel = channelIndex;
}

void screens::bandScanMode(uint8_t state) {
    reset(); // start from fresh screen.
    best_rssi = 0;
    if(state==STATE_SCAN)
    {
        drawTitleBox(PSTR("BAND SCANNER"));
    }
    else
    {
        drawTitleBox(PSTR("RSSI SETUP"));
    }
    TV.select_font(font8x8);
    if(state==STATE_SCAN)
    {
        TV.select_font(font4x6);
        TV.draw_line(50,1*TV_Y_GRID,50, 1*TV_Y_GRID+9,WHITE);
        TV.printPGM(2, SCANNER_LIST_Y_POS, PSTR("BEST:"));
    }
    else
    {
        TV.select_font(font4x6);
        TV.printPGM(10, SCANNER_LIST_Y_POS, PSTR("RSSI Min:     RSSI Max:   "));
    }
    TV.draw_rect(0,1*TV_Y_GRID,TV_X_MAX,9,  WHITE); // list frame
    TV.draw_rect(0,TV_ROWS - TV_SCANNER_OFFSET,TV_X_MAX,13,  WHITE); // lower frame
    TV.select_font(font4x6);
#ifdef USE_LBAND
    TV.printPGM(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), PSTR("5362"));
#else
    TV.printPGM(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), PSTR("5645"));
#endif
    TV.printPGM(57, (TV_ROWS - TV_SCANNER_OFFSET + 2), PSTR("5800"));
    TV.printPGM(111, (TV_ROWS - TV_SCANNER_OFFSET + 2), PSTR("5945"));
}

void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {
    // force tune on new scan start to get right RSSI value
    static uint8_t writePos=SCANNER_LIST_X_POS;
    // channel marker
    if(channel != last_channel) // only updated on changes
    {
        // clear last square
        TV.draw_rect(1, (TV_ROWS - TV_SCANNER_OFFSET + 8),125,SCANNER_MARKER_SIZE,  BLACK, BLACK);
#ifdef USE_LBAND
        // draw next
        TV.draw_rect((channel * 5/2)+5, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);
#else
        // draw next
        TV.draw_rect((channel * 3)+5, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);
#endif
    }
    // print bar for spectrum

    uint8_t rssi_scaled=map(rssi, 1, 100, 5, SCANNER_BAR_SIZE);
    // clear last bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_SIZE)-5, 2, SCANNER_BAR_SIZE+5 , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), 2, rssi_scaled , WHITE, WHITE);
    // print channelname

    if(!in_setup) {
        if (rssi > RSSI_SEEK_TRESHOLD) {
            if(best_rssi < rssi) {
                best_rssi = rssi;
                TV.print(22, SCANNER_LIST_Y_POS, channelName, HEX);
                TV.print(32, SCANNER_LIST_Y_POS, channelFrequency);
            }
            else {
                if(writePos+10>TV_COLS-2)
                { // keep writing on the screen
                    writePos=SCANNER_LIST_X_POS;
                }
                TV.draw_rect(writePos, SCANNER_LIST_Y_POS, 8, 6,  BLACK, BLACK);
                TV.print(writePos, SCANNER_LIST_Y_POS, channelName, HEX);
                writePos += 10;
            }
            TV.draw_rect((channel * 3) - 5, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled) - 5, 8, 7,  BLACK, BLACK);
            TV.print((channel * 3) - 4, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled) - 5, channelName, HEX);
        }
    }
    else {
            TV.print(50, SCANNER_LIST_Y_POS, "   ");
            TV.print(50, SCANNER_LIST_Y_POS, rssi_setup_min_a , DEC);

            TV.print(110, SCANNER_LIST_Y_POS, "   ");
            TV.print(110, SCANNER_LIST_Y_POS, rssi_setup_max_a , DEC);
    }

    last_channel = channel;
}

void screens::screenSaver(uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    screenSaver(-1, channelName, channelFrequency, call_sign);
}
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
 // not used in TVOut ... yet
/*    reset();
    TV.select_font(font8x8);
    TV.print(0,0,channelName, HEX);
    TV.select_font(font6x8);
    TV.print(70,0,CALL_SIGN);
    TV.select_font(font8x8);
    TV.print(70,28,channelFrequency);
    TV.select_font(font4x6);
#ifdef USE_DIVERSITY
    switch(diversity_mode) {
        case useReceiverAuto:
            TV.print(70,18,"AUTO");
            break;
        case useReceiverA:
            TV.print(70,18,"ANTENNA A");
            break;
        case useReceiverB:
            TV.print(70,18,"ANTENNA B");
            break;
    }
    TV.draw_rect(0, 127-19, 7, 9, WHITE,BLACK);
    TV.print(1,95-18,"A");
    TV.draw_rect(0, 127-9, 7, 9, WHITE,BLACK);
    TV.print(1,95-8,"B");
#endif
*/
}

void screens::updateScreenSaver(uint8_t rssi) {
    updateScreenSaver(-1, rssi, -1, -1);
}
void screens::updateScreenSaver(char active_receiver, uint8_t rssi, uint8_t rssiA, uint8_t rssiB) {
// not used in TVOut ... yet
}
#ifdef USE_VOLTAGE_MONITORING
void screens::updateVoltageScreenSaver(int voltage, boolean alarm){
// not used in TVOut ... yet
}
#endif

#ifdef USE_DIVERSITY
void screens::diversity(uint8_t diversity_mode) {
    reset();
    drawTitleBox(PSTR("DIVERSITY"));
    TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Auto"));
    TV.printPGM(10, 5+2*MENU_Y_SIZE, PSTR("Receiver A"));
    TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("Receiver B"));
    // RSSI Strength
    TV.draw_line(0,3+4*MENU_Y_SIZE, TV_X_MAX, 3+4*MENU_Y_SIZE, WHITE);
    TV.printPGM(10, 6+4*MENU_Y_SIZE, PSTR("A:"));
    TV.draw_line(0,3+5*MENU_Y_SIZE, TV_X_MAX, 3+5*MENU_Y_SIZE, WHITE);
    TV.printPGM(10, 6+5*MENU_Y_SIZE, PSTR("B:"));

    TV.draw_rect(0,3+(diversity_mode+1)*MENU_Y_SIZE,127,12,  WHITE, INVERT);
}
void screens::updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB){
    #define RSSI_BAR_SIZE 100
    uint8_t rssi_scaled=map(rssiA, 1, 100, 1, RSSI_BAR_SIZE);
    // clear last bar
    TV.draw_rect(25+rssi_scaled, 6+4*MENU_Y_SIZE, RSSI_BAR_SIZE-rssi_scaled, 8 , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect(25, 6+4*MENU_Y_SIZE, rssi_scaled, 8 , WHITE, (active_receiver==useReceiverA ? WHITE:BLACK));

    // read rssi B
    rssi_scaled=map(rssiB, 1, 100, 1, RSSI_BAR_SIZE);
    // clear last bar
    TV.draw_rect(25+rssi_scaled, 6+5*MENU_Y_SIZE, RSSI_BAR_SIZE-rssi_scaled, 8 , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect(25, 6+5*MENU_Y_SIZE, rssi_scaled, 8 , WHITE, (active_receiver==useReceiverB ? WHITE:BLACK));

}
#endif

#ifdef USE_VOLTAGE_MONITORING
void screens::voltage(uint8_t menu_id, int voltage_calibration, uint8_t warning_voltage, uint8_t critical_voltage) {
    reset();
    drawTitleBox(PSTR("VOLTAGE ALARM"));
    TV.printPGM(5, 5+1*MENU_Y_SIZE, PSTR("Warning"));
    TV.print(5+(11*8), 5+1*MENU_Y_SIZE, (float)warning_voltage/10.0, 1);
    TV.printPGM(5, 5+2*MENU_Y_SIZE, PSTR("Critical"));
    TV.print(5+(11*8), 5+2*MENU_Y_SIZE, (float)critical_voltage/10.0, 1);
    TV.printPGM(5, 5+3*MENU_Y_SIZE, PSTR("Calibrate"));
    TV.print(5+(11*8), 5+3*MENU_Y_SIZE, voltage_calibration, 10);
    TV.printPGM(5, 5+4*MENU_Y_SIZE, PSTR("Save"));

    TV.draw_rect(0,3+(menu_id+1)*MENU_Y_SIZE,127,12,  WHITE, INVERT);
}
void screens::updateVoltage(int voltage){

    TV.printPGM(5, 10+5*MENU_Y_SIZE, PSTR("Measured"));
    TV.print(5+(11*8), 10+5*MENU_Y_SIZE, (float)voltage/10, 1);

}
#endif

void screens::setupMenu(){
}
void screens::updateSetupMenu(uint8_t menu_id,bool settings_beeps,bool settings_orderby_channel, const char *call_sign, char editing){
    reset();
    drawTitleBox(PSTR("SETUP MENU"));

    TV.printPGM(5, 5+1*MENU_Y_SIZE, PSTR("ORDER: "));
    if(settings_orderby_channel) {
        TV.printPGM(5+(6*8), 5+1*MENU_Y_SIZE, PSTR("CHANNEL  "));
    }
    else {
        TV.printPGM(5+(6*8), 5+1*MENU_Y_SIZE, PSTR("FREQUENCY"));
    }


    TV.printPGM(5, 5+2*MENU_Y_SIZE, PSTR("BEEPS: "));
    if(settings_beeps) {
        TV.printPGM(5+(6*8), 5+2*MENU_Y_SIZE, PSTR("ON "));
    }
    else {
        TV.printPGM(5+(6*8), 5+2*MENU_Y_SIZE, PSTR("OFF"));
    }


/* NO NEED FOR CALL SIGN IN TVOUT MODE
    TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("SIGN : "));
    if(editing>=0) {
        for(uint8_t i=0; i<10; i++) {
            TV.print(5+((7+i)*8), 5+3*MENU_Y_SIZE, call_sign[i]);
        }

        TV.draw_rect(5+((7)*8),3+(menu_id+1)*MENU_Y_SIZE,TV_X_MAX-(5+((7)*8)),12,  WHITE, INVERT);
        TV.draw_rect(5+((7+editing)*8),3+(menu_id+1)*MENU_Y_SIZE,8,12,  BLACK, INVERT);
    }
    else {
        TV.print(5+(6*8), 5+3*MENU_Y_SIZE, call_sign);
    }
*/

    TV.printPGM(5, 5+4*MENU_Y_SIZE, PSTR("CALIBRATE RSSI"));
#ifdef USE_VOLTAGE_MONITORING
    TV.printPGM(5, 5+5*MENU_Y_SIZE, PSTR("VOLTAGE ALARM"));

    TV.printPGM(5, 5+6*MENU_Y_SIZE, PSTR("SAVE & EXIT"));
#else
    TV.printPGM(5, 5+5*MENU_Y_SIZE, PSTR("SAVE & EXIT"));
#endif

    TV.draw_rect(0,3+(menu_id+1)*MENU_Y_SIZE,127,12,  WHITE, INVERT);
}

void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency, const char *call_sign) {
    reset();
    drawTitleBox(PSTR("SAVE SETTINGS"));
    TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Mode:"));
    switch (mode)
    {
        case STATE_SCAN: // Band Scanner
            TV.printPGM(50,5+1*MENU_Y_SIZE,  PSTR("Scanner"));
        break;
        case STATE_MANUAL: // manual mode
            TV.printPGM(50,5+1*MENU_Y_SIZE,  PSTR("Manual"));
        break;
        case STATE_SEEK: // seek mode
            TV.printPGM(50,5+1*MENU_Y_SIZE,  PSTR("Search"));
        break;
    }
    TV.printPGM(10, 5+2*MENU_Y_SIZE, PSTR("Band:"));
    // print band
    if(channelIndex > 39)
    {
        TV.printPGM(50,5+2*MENU_Y_SIZE,  PSTR("D/5.3    "));
    }
    else if(channelIndex > 31)
    {
        TV.printPGM(50,5+2*MENU_Y_SIZE,  PSTR("C/Race   "));
    }
    else if(channelIndex > 23)
    {
        TV.printPGM(50,5+2*MENU_Y_SIZE,  PSTR("F/Airwave"));
    }
    else if (channelIndex > 15)
    {
        TV.printPGM(50,5+2*MENU_Y_SIZE,  PSTR("E        "));
    }
    else if (channelIndex > 7)
    {
        TV.printPGM(50,5+2*MENU_Y_SIZE,  PSTR("B        "));
    }
    else
    {
        TV.printPGM(50,5+2*MENU_Y_SIZE,  PSTR("A        "));
    }
    TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("Chan:"));
    uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE+1; // get channel inside band
    TV.print(50,5+3*MENU_Y_SIZE,active_channel,DEC);
    TV.printPGM(10, 5+4*MENU_Y_SIZE, PSTR("FREQ:     GHz"));
    TV.print(50,5+4*MENU_Y_SIZE, channelFrequency);
    TV.printPGM(10, 5+5*MENU_Y_SIZE, PSTR("--- SAVED ---"));
}

void screens::updateSave(const char * msg) {
    TV.select_font(font4x6);
    TV.print(((127-strlen(msg)*4)/2), 14+5*MENU_Y_SIZE, msg);
}


#endif
