#ifndef display_h
#define display_h

//Each screen has the following
// public entry method
// private draw method
// public update method
// private update draw method

class screens
{
    private: // helper functions for screens.
        void reset();
        void drawMainMenu(char menu_id);
        void drawTitleBox(const char *title);
        void drawSeekMode(char state);
        void drawBandScanMode(char state);
        void drawUpdateBandScanMode(char state, char channel, char rssi, int channelName, int channelFrequency, int rssi_setup_min_a, int rssi_setup_max_a);


    public:
        screens();
        void begin();
        void mainMenu(char menu_id);
        void seekMode(char state); // seek and manual mode
        void bandScanMode(char state);
        void updateBandScanMode(char state, char channel, char rssi, int channelName, int channelFrequency, int rssi_setup_min_a, int rssi_setup_max_a);
        void flip();
};
#endif
