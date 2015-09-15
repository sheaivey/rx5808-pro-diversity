#include "settings.h"

#ifdef TVOUT_SCREENS
#include "screens.h" // function headers
#include <arduino.h>


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

#define MENU_Y_SIZE 15
#define TV_Y_GRID 14
#define TV_Y_OFFSET 3


TVout TV;

screens::screens() {
    last_channel = -1;
    last_rssi = 0;
}

char screens::begin() {
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
    TV.print(((127-strlen(title)*8)/2), 3,  title);
    TV.draw_rect(0,0,127,14,  WHITE,INVERT);
}

void screens::mainMenu(uint8_t menu_id) {
    reset(); // start from fresh screen.
    drawTitleBox("MODE SELECTION");

    TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Auto Search"));
    TV.printPGM(10, 5+2*MENU_Y_SIZE, PSTR("Band Scanner"));
    TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("Manual Mode"));
#ifdef USE_DIVERSITY
    TV.printPGM(10, 5+4*MENU_Y_SIZE, PSTR("Diversity"));
#endif
    TV.printPGM(10, 5+5*MENU_Y_SIZE, PSTR("Save Setup"));
    // selection by inverted box

    TV.draw_rect(0,3+(menu_id+1)*MENU_Y_SIZE,127,12,  WHITE, INVERT);
}

void screens::seekMode(uint8_t state) {
    last_channel = -1;
    reset(); // start from fresh screen.
    if (state == STATE_MANUAL)
    {
        drawTitleBox("MANUAL MODE");
    }
    else if(state == STATE_SEEK)
    {
        drawTitleBox("AUTO SEEK MODE");
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
    TV.print(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5645");
    TV.print(57, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5800");
    TV.print(111, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5945");

}

void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, bool locked) {
    // display refresh handler
    TV.select_font(font8x8);
    if(channelIndex != last_channel) // only updated on changes
    {
        // show current used channel of bank
        if(channelIndex > 31)
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
        TV.draw_rect((channel * 3)+5, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);

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
    // clear last bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_MINI_SIZE), 2, SCANNER_BAR_MINI_SIZE , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), 2, rssi_scaled , WHITE, WHITE);
    // handling for seek mode after screen and RSSI has been fully processed
    if(state == STATE_SEEK)
    { // SEEK MODE

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

    last_channel = channelIndex;
}

void screens::bandScanMode(uint8_t state) {
    reset(); // start from fresh screen.
    if(state==STATE_SCAN)
    {
        drawTitleBox("BAND SCANNER");
    }
    else
    {
        drawTitleBox("RSSI SETUP");
    }
    TV.select_font(font8x8);
    if(state==STATE_SCAN)
    {
        TV.select_font(font4x6);
        TV.print(2, SCANNER_LIST_Y_POS, "BEST:");
    }
    else
    {
        TV.select_font(font4x6);
        TV.print(10, SCANNER_LIST_Y_POS, "RSSI Min:     RSSI Max:   ");
    }
    TV.draw_rect(0,1*TV_Y_GRID,TV_X_MAX,9,  WHITE); // list frame
    TV.draw_rect(0,TV_ROWS - TV_SCANNER_OFFSET,TV_X_MAX,13,  WHITE); // lower frame
    TV.select_font(font4x6);
    TV.print(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5645");
    TV.print(57, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5800");
    TV.print(111, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5945");
}

void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {
    // force tune on new scan start to get right RSSI value

    // channel marker
    if(channel != last_channel) // only updated on changes
    {
        // clear last square
        TV.draw_rect(1, (TV_ROWS - TV_SCANNER_OFFSET + 8),125,SCANNER_MARKER_SIZE,  BLACK, BLACK);
        // draw next
        TV.draw_rect((channel * 3)+5, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);
    }
    // print bar for spectrum

    uint8_t rssi_scaled=map(rssi, 1, 100, 5, SCANNER_BAR_SIZE);
    // clear last bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_SIZE), 2, SCANNER_BAR_SIZE , BLACK, BLACK);
    //  draw new bar
    TV.draw_rect((channel * 3)+4, (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled), 2, rssi_scaled , WHITE, WHITE);
    // print channelname

    if(!in_setup) {
        if(channelName < 255) {
            TV.print(22, SCANNER_LIST_Y_POS, channelName, HEX);
            TV.print(32, SCANNER_LIST_Y_POS, channelFrequency);
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

void screens::screenSaver(uint8_t channelName, uint16_t channelFrequency) {
    screenSaver(-1, channelName, channelFrequency);
}
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency) {
    /*
    reset();
    display.setTextSize(6);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(channelName, HEX);
    display.setTextSize(1);
    display.setCursor(70,0);
    display.print(CALL_SIGN);
    display.setTextSize(2);
    display.setCursor(70,28);
    display.setTextColor(WHITE);
    display.print(channelFrequency);
    display.setTextSize(1);
#ifdef USE_DIVERSITY
    display.setCursor(70,18);
    switch(diversity_mode) {
        case useReceiverAuto:
            display.print("AUTO");
            break;
        case useReceiverA:
            display.print("ANTENNA A");
            break;
        case useReceiverB:
            display.print("ANTENNA B");
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
#endif
    display.display();
    */
}

void screens::updateScreenSaver(uint8_t rssi) {
    updateScreenSaver(-1, rssi, -1, -1);
}
void screens::updateScreenSaver(char active_receiver, uint8_t rssi, uint8_t rssiA, uint8_t rssiB) {
/*
#ifdef USE_DIVERSITY
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
#else
    display.setTextColor(BLACK);
    display.fillRect(0, display.height()-19, 25, 19, WHITE);
    display.setCursor(1,display.height()-13);
    display.print("RSSI");
    #define RSSI_BAR_SIZE 101
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
    display.fillRect(25 + rssi_scaled, display.height()-19, (RSSI_BAR_SIZE-rssi_scaled), 19, BLACK);
    display.fillRect(25, display.height()-19, rssi_scaled, 19, WHITE);
#endif
    if(rssi < 20)
    {
        display.setTextColor((millis()%250 < 125) ? WHITE : BLACK, BLACK);
        display.setCursor(50,display.height()-13);
        display.print("LOW SIGNAL");
    }
#ifdef USE_DIVERSITY
    else {
        display.drawLine(50,display.height()-10,110,display.height()-10,BLACK);
    }
#endif
    display.display();
    */
}

#ifdef USE_DIVERSITY
void screens::diversity(uint8_t diversity_mode) {
    reset();
    drawTitleBox("DIVERSITY");
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

void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency) {
    reset();
    drawTitleBox("SAVE SETTINGS");
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
    if(channelIndex > 31)
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
