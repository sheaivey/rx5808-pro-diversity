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

#ifdef USE_CUSTOM_BOOT_LOGO
  #include "Boot_Logo.h"
#endif

#ifdef OLED_128x64_ADAFRUIT_SCREENS
#include "screens.h" // function headers
#ifdef SH1106
	#include <Adafruit_SH1106.h>
#else
	#include <Adafruit_SSD1306.h>
#endif
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

// New version of PSTR that uses a temp buffer and returns char *
// by Shea Ivey
#define PSTR2(x) PSTRtoBuffer_P(PSTR(x))
char PSTR2_BUFFER[30]; // adjust size depending on need.
char *PSTRtoBuffer_P(PGM_P str) { uint8_t c='\0', i=0; for(; (c = pgm_read_byte(str)) && i < sizeof(PSTR2_BUFFER); str++, i++) PSTR2_BUFFER[i]=c;PSTR2_BUFFER[i]=c; return PSTR2_BUFFER;}

#define INVERT INVERSE
#define OLED_RESET 4
#ifdef SH1106
	Adafruit_SH1106 display(OLED_RESET);
	#if !defined SH1106_128_64
		#error("Screen size incorrect, please fix Adafruit_SH1106.h!");
	#endif
#else
	Adafruit_SSD1306 display(OLED_RESET);
	#if !defined SSD1306_128_64
		#error("Screen size incorrect, please fix Adafruit_SSD1306.h!");
	#endif
#endif


screens::screens() {
    last_channel = -1;
    last_rssi = 0;
}

char screens::begin(const char *call_sign) {
    // Set the address of your OLED Display.
    // 128x64 ONLY!!
#ifdef SH1106
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
#else
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
#endif


#ifdef USE_FLIP_SCREEN
    flip();
#endif

#ifdef USE_BOOT_LOGO
    display.display(); // show splash screen
    delay(3000);
#endif
#ifdef USE_CUSTOM_BOOT_LOGO
    display.clearDisplay();
    display.drawBitmap(0, 0,  LOGO_LCD, 128, 64, WHITE);     //( X start pos, Y start pos, IMAGE, Widht dimension, Height Dimension, COLOR)
    display.display();
    delay (3000);
#endif
    // init done
    reset();

    display.fillRect(0, 0, display.width(), 11,WHITE);
    display.setTextColor(BLACK);
    display.setCursor(((display.width() - (10*6)) / 2),2);
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
    delay(250);
    display.setCursor(display.width()-6*8,8*2+4);
    if(isDiversity()) {
        display.print(PSTR2(" ENABLED"));
    }
    else {
        display.print(PSTR2("DISABLED"));
    }
#endif
    display.setCursor(((display.width() - (strlen(call_sign)*12)) / 2),8*4+4);
    display.setTextSize(2);
    display.print(call_sign);
    display.display();
    delay(1250);
    return 0; // no errors
}

void screens::reset() {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void screens::flip() {
    display.setRotation(2);
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

void screens::drawBottomTriangle(bool color){
    //use fillRect instead of fillTriangle
    display.fillRect(120, 58, 5, 1, color);
    display.fillRect(121, 59, 3, 1, color);
    display.fillRect(122, 60, 1, 1, color);
}

void screens::drawTopTriangle(bool color){
    //use fillRect instead of fillTriangle
    display.fillRect(120, 14, 5, 1, color);
    display.fillRect(121, 13, 3, 1, color);
    display.fillRect(122, 12, 1, 1, color);
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
    display.print(PSTR2("SETUP MENU"));

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
#ifdef USE_LBAND
    display.print(PSTR2("5362"));
#else
    display.print(PSTR2("5645"));
#endif
    display.setCursor(55,display.height()-9);
    display.print(PSTR2("5800"));
    display.setCursor(display.width()-25,display.height()-9);
    display.print(PSTR2("5945"));
    display.display();
}

char scan_position = 3;

void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, uint8_t rssi_seek_threshold, bool locked) {
    // display refresh handler
    if(channel != last_channel) // only updated on changes
    {
        if(channel > last_channel) {
            scan_position = 3;
        }
        else {
            scan_position = -1;
        }
        display.setTextColor(WHITE,BLACK);
        display.setCursor(36,12);
        // show current used channel of bank
#ifdef USE_LBAND
        if(channelIndex > 39)
        {
            display.print(PSTR2("D/5.3    "));
        }
        else if(channelIndex > 31)
#else
	if(channelIndex > 31)
#endif
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
#ifdef USE_LBAND
    display.fillRect((channel*3)+4,display.height()-12-14,5/2,14-rssi_scaled,BLACK);
    display.fillRect((channel*3)+4,(display.height()-12-rssi_scaled),5/2,rssi_scaled,WHITE);
#else
    display.fillRect((channel*3)+4,display.height()-12-14,3,14-rssi_scaled,BLACK);
    display.fillRect((channel*3)+4,(display.height()-12-rssi_scaled),3,rssi_scaled,WHITE);
#endif

    // handling for seek mode after screen and RSSI has been fully processed
    if(state == STATE_SEEK) //
    { // SEEK MODE

        // Show Scan Position
#ifdef USE_LBAND
        display.fillRect((channel*5/2)+4+scan_position,display.height()-12-14,1,14,BLACK);
#else
        display.fillRect((channel*3)+4+scan_position,display.height()-12-14,1,14,BLACK);
#endif

        rssi_scaled=map(rssi_seek_threshold, 1, 100, 1, 14);

        display.fillRect(1,display.height()-12-14,2,14,BLACK);
        display.drawLine(1,display.height()-12-rssi_scaled,2,display.height()-12-rssi_scaled, WHITE);
        display.fillRect(display.width()-3,display.height()-12-14,2,14,BLACK);
        display.drawLine(display.width()-3,display.height()-12-rssi_scaled,display.width(),display.height()-12-rssi_scaled, WHITE);

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
#ifdef USE_LBAND
    display.print(PSTR2("5362"));
#else
    display.print(PSTR2("5645"));
#endif
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
#ifdef USE_LBAND
        display.fillRect((channel*5/2)+4,display.height()-12-30,5/2,30-rssi_scaled,BLACK);
        display.fillRect((channel*5/2)+4,hight,5/2,rssi_scaled,WHITE);
        // Show Scan Position
        display.fillRect((channel*5/2)+4+3,display.height()-12-30,1,30,BLACK);
#else
        display.fillRect((channel*3)+4,display.height()-12-30,3,30-rssi_scaled,BLACK);
        display.fillRect((channel*3)+4,hight,3,rssi_scaled,WHITE);
        // Show Scan Position
        display.fillRect((channel*3)+4+3,display.height()-12-30,1,30,BLACK);
#endif
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

void screens::screenSaver(uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    screenSaver(-1, channelName, channelFrequency, call_sign);
}
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    reset();
    display.setTextSize(6);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(channelName, HEX);
    display.setTextSize(1);
    display.setCursor(70,0);
    display.print(call_sign);
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
#ifndef USE_VOLTAGE_MONITORING
    display.display();
#endif
}

#ifdef USE_VOLTAGE_MONITORING
void screens::updateVoltageScreenSaver(int voltage, bool alarm){
    if(alarm){
        display.setTextColor((millis()%250 < 125) ? WHITE : BLACK, BLACK);
    } else {
        display.setTextColor(INVERT);
    }
    display.setCursor(70,9);
    display.print((float)voltage/10.0);
    display.print(PSTR2("V"));
    display.setTextColor(BLACK);
    display.display();

}
#endif

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

#ifdef USE_VOLTAGE_MONITORING
void screens::voltage(uint8_t menu_id, int voltage_calibration, uint8_t warning_voltage, uint8_t critical_voltage) {
    reset();
    drawTitleBox(PSTR2("VOLTAGE ALARM"));

    display.fillRect(0, 10*menu_id+12, display.width(), 10, WHITE);

    display.setTextColor(menu_id == 0 ? BLACK : WHITE);
    display.setCursor(5,10*1+3);
    display.print(PSTR2("Warning:"));
    display.setCursor(80 ,10*1+3);
    display.print((float)warning_voltage/10.0);

    display.setTextColor(menu_id == 1 ? BLACK : WHITE);
    display.setCursor(5,10*2+3);
    display.print(PSTR2("Critical:"));
    display.setCursor(80 ,10*2+3);
    display.print((float)critical_voltage/10.0);

    display.setTextColor(menu_id == 2 ? BLACK : WHITE);
    display.setCursor(5,10*3+3);
    display.print(PSTR2("Calibrate:"));
    display.setCursor(80 ,10*3+3);
    display.print(voltage_calibration);

    display.setTextColor(menu_id == 3 ? BLACK : WHITE);
    display.setCursor(5,10*4+3);
    display.print(PSTR2("Save"));

    display.setTextColor(WHITE);
    display.setCursor(5,10*5+3);
    display.print(PSTR2("Measured:"));

    display.display();
}
void screens::updateVoltage(int voltage){

    display.fillRect(80, 53, 40, 10, BLACK);
    display.setTextColor(WHITE);
    display.setCursor(80 ,10*5+3);
    //instaed of resetiing the whole display - black out the value
    display.print((float)voltage/10.0);
    display.setTextColor(BLACK);
    display.display();

}
#endif

void screens::setupMenu(){
}
void screens::updateSetupMenu(uint8_t menu_id, bool settings_beeps, bool settings_orderby_channel, const char *call_sign, char editing){
    reset();
    drawTitleBox(PSTR2("SETUP MENU"));
    //selected
    int selected_position = menu_id % 5;
    display.fillRect(0, 10*selected_position+12, display.width(), 10, WHITE);
    if(menu_id < 5){
        drawBottomTriangle(selected_position == 4 ? BLACK : WHITE);
        display.setTextColor(selected_position == 0 ? BLACK : WHITE);
        display.setCursor(5,10*1+3);
        display.print(PSTR2("ORDER: "));
        if(settings_orderby_channel) {
            display.print(PSTR2("CHANNEL  "));
        }
        else {
            display.print(PSTR2("FREQUENCY"));
        }

        display.setTextColor(selected_position == 1 ? BLACK : WHITE);
        display.setCursor(5,10*2+3);
        display.print(PSTR2("BEEPS: "));
        if(settings_beeps) {
            display.print(PSTR2("ON "));
        }
        else {
            display.print(PSTR2("OFF"));
        }


        display.setTextColor(selected_position == 2 ? BLACK : WHITE);
        display.setCursor(5,10*3+3);
        display.print(PSTR2("SIGN : "));
        if(editing>=0) {
            display.fillRect(6*6+5, 10*2+13, display.width()-(6*6+6), 8, BLACK);
            display.fillRect(6*7+6*(editing)+4, 10*2+13, 7, 8, WHITE); //set cursor
            for(uint8_t i=0; i<10; i++) {
                display.setTextColor(i == editing ? BLACK : WHITE);
                display.print(call_sign[i]);
            }
        }
        else {
            display.print(call_sign);
        }

        display.setTextColor(selected_position == 3 ? BLACK : WHITE);
        display.setCursor(5,10*4+3);
        display.print(PSTR2("CALIBRATE RSSI"));

#ifdef USE_VOLTAGE_MONITORING
        display.setTextColor(selected_position == 4 ? BLACK : WHITE);
        display.setCursor(5,10*5+3);
        display.print(PSTR2("VOLTAGE ALARM"));
    } else {
        drawTopTriangle(selected_position == 0 ? BLACK : WHITE);
        display.setTextColor(selected_position == 0 ? BLACK : WHITE);
        display.setCursor(5,10*1+3);
        display.print(PSTR2("SAVE & EXIT"));
    }
#else
        display.setTextColor(selected_position == 4 ? BLACK : WHITE);
        display.setCursor(5,10*5+3);
        display.print(PSTR2("SAVE & EXIT"));
    }
#endif
    display.display();
}

void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency,const char *call_sign) {
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
#ifdef USE_LBAND
    if(channelIndex > 39)
    {
        display.print(PSTR2("D/5.3    "));
    }
    else if(channelIndex > 31)
#else
    if(channelIndex > 31)
#endif
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

    display.setCursor(5,8*5+4);
    display.print(PSTR2("SIGN:"));
    display.setCursor(38,8*5+4);
    display.print(call_sign);

    display.setCursor(((display.width()-11*6)/2),8*6+4);
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
