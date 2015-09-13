#ifndef display_h
#define display_h


#include <avr/pgmspace.h>

//Each screen has the following
// public entry method
// private draw method
// public update method
// private update draw method

class screens
{
    private: // helper functions for screens.

        uint8_t last_rssi;
        uint8_t last_channel;

        void reset();
        void drawTitleBox(const char *title);

        // MAIN MENU
        void drawMainMenu(uint8_t menu_id);

        // SEEK & MANUAL MODE
        void drawSeekMode(uint8_t state);
        void drawUpdateSeekMode(uint8_t state, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, bool locked);

        // BAND SCAN
        void drawBandScanMode(uint8_t state);
        void drawUpdateBandScanMode(uint8_t state, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a);

        // SCREEN SAVER
        void drawScreenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency);
        void drawUpdateScreenSaver(char active_receiver, uint8_t rssi, uint8_t rssiA, uint8_t rssiB);

        // DIVERSITY

        // SAVE


    public:
        screens();
        void begin();
        void flip();

        // MAIN MENU
        void mainMenu(uint8_t menu_id);

        // SEEK & MANUAL MODE
        void seekMode(uint8_t state); // seek and manual mode
        void updateSeekMode(uint8_t state, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, bool locked); // seek and manual mode

        // BAND SCAN
        void bandScanMode(uint8_t state);
        void updateBandScanMode(uint8_t state, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a);

        // SCREEN SAVER
        void screenSaver(uint8_t channelName, uint16_t channelFrequency);
        void screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency);
        void updateScreenSaver(uint8_t rssi);
        void updateScreenSaver(char active_receiver, uint8_t rssi, uint8_t rssiA, uint8_t rssiB); // diversity

        // DIVERSITY

        // SAVE
};
#endif
