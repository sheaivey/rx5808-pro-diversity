#include "settings.h"

#ifdef TVOUT_SCREENS
#include "screens.h" // function headers

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


TVout TV;

screens::screens() {
    last_channel = -1;
    last_rssi = 0;
}

char screens::begin() {
    return TV.begin(TV_FORMAT, TV_COLS, TV_ROWS);
}

void screens::reset() {

}

void screens::flip() {
}

void screens::drawTitleBox(const char *title) {

}

void screens::mainMenu(uint8_t menu_id) {
    reset(); // start from fresh screen.
    drawTitleBox("MODE SELECTION");
}

void screens::seekMode(uint8_t state) {
    last_channel = -1;
    reset(); // start from fresh screen.
}

void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, bool locked) {

    last_channel = channel;
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
}

void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {

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
}
void screens::updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB){

}
#endif

void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency) {
    reset();
    drawTitleBox("SAVE SETTINGS");
}

void screens::updateSave(const char * msg) {
    /*
    display.setTextColor(WHITE,BLACK);
    display.setCursor(((display.width()-strlen(msg)*6)/2),8*6+4);
    display.print(msg);
    display.display();
    */
}


#endif
