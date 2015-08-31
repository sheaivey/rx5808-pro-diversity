/*
 * SPI driver based on fs_skyrf_58g-main.c Written by Simon Chambers
 * TVOUT by Myles Metzel
 * Scanner by Johan Hermen
 * Inital 2 Button version by Peter (pete1990)
 * Refactored and GUI reworked by Marko Hoepken
 * Universal version my Marko Hoepken
 * Diversity Receiver Mode and GUI improvements by Shea Ivey
 * OLED Version by Shea Ivey

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

//#include <TVout.h>
//#include <fontALL.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Feature Togglels
//#define DEBUG
#define USE_DIVERSITY


#define spiDataPin 10
#define slaveSelectPin 11
#define spiClockPin 12

// Receiver PINS
#define receiverA_led A0
#define rssiPinA A6

#ifdef USE_DIVERSITY
    // Diversity
    #define receiverB_led A1
    #define rssiPinB A7
    #define useReceiverAuto 0
    #define useReceiverA 1
    #define useReceiverB 2
    // rssi strenth should be 2% greater than other receiver before switch.
    // this pervents flicker when rssi are close.
    #define DIVERSITY_CUTOVER 2
#endif

// this two are minimum required
#define buttonSeek 2
#define buttonMode 3
// optional comfort buttons
#define buttonDown 4
#define buttonSave 5
// Buzzer
#define buzzer 6

// key debounce delay in ms
// NOTE: good values are in the range of 100-200ms
// shorter values will make it more reactive, but may lead to double trigger
#define KEY_DEBOUNCE 200

#define led 13
// RSSI default raw range
#define RSSI_MIN_VAL 90
#define RSSI_MAX_VAL 300
// 75% threshold, when channel is printed in spectrum
#define RSSI_SEEK_FOUND 75
// 80% under max value for RSSI
#define RSSI_SEEK_TRESHOLD 80
// scan loops for setup run
#define RSSI_SETUP_RUN 10

#define STATE_SEEK_FOUND 0
#define STATE_SEEK 1
#define STATE_SCAN 2
#define STATE_MANUAL 3
#ifdef USE_DIVERSITY
#define STATE_DIVERSITY 4
#endif
#define STATE_SAVE 5
#define STATE_RSSI_SETUP 6

#define START_STATE STATE_SEEK
#define MAX_STATE STATE_MANUAL

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN_INDEX 0
#define CHANNEL_MAX_INDEX 39

#define CHANNEL_MAX 39
#define CHANNEL_MIN 0

#define EEPROM_ADR_STATE 0
#define EEPROM_ADR_TUNE 1
#define EEPROM_ADR_RSSI_MIN_A_L 2
#define EEPROM_ADR_RSSI_MIN_A_H 3
#define EEPROM_ADR_RSSI_MAX_A_L 4
#define EEPROM_ADR_RSSI_MAX_A_H 5
#ifdef USE_DIVERSITY
#define EEPROM_ADR_DIVERSITY 6
#define EEPROM_ADR_RSSI_MIN_B_L 7
#define EEPROM_ADR_RSSI_MIN_B_H 8
#define EEPROM_ADR_RSSI_MAX_B_L 9
#define EEPROM_ADR_RSSI_MAX_B_H 10
#endif

// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
  0x2A05,    0x299B,    0x2991,    0x2987,    0x291D,    0x2913,    0x2909,    0x289F,    // Band A
  0x2903,    0x290C,    0x2916,    0x291F,    0x2989,    0x2992,    0x299C,    0x2A05,    // Band B
  0x2895,    0x288B,    0x2881,    0x2817,    0x2A0F,    0x2A19,    0x2A83,    0x2A8D,    // Band E
  0x2906,    0x2910,    0x291A,    0x2984,    0x298E,    0x2998,    0x2A02,    0x2A0C,    // Band F / Airwave
  0x281D,    0x288F,    0x2902,    0x2914,    0x2978,    0x2999,    0x2A0C,    0x2A1E     // Band C / Immersion Raceband
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band C / Immersion Raceband
};

// do coding as simple hex value to save memory.
const uint8_t channelNames[] PROGMEM = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // Band A
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, // Band B
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // Band E
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, // Band F / Airwave
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8  // Band C / Immersion Raceband
};

// All Channels of the above List ordered by Mhz
const uint8_t channelList[] PROGMEM = {
  19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
};

uint8_t channel = 0;
uint8_t channelIndex = 0;
uint8_t rssi = 0;
uint8_t rssi_scaled = 0;
#ifdef USE_DIVERSITY
uint8_t diversity_mode = useReceiverAuto;
#endif
uint8_t hight = 0;
uint8_t state = START_STATE;
uint8_t state_last_used=START_STATE;
uint8_t last_state= START_STATE+1; // force screen draw
uint8_t writePos = 0;
uint8_t switch_count = 0;
uint8_t man_channel = 0;
uint8_t last_channel_index = 0;
uint8_t force_seek=0;
unsigned long time_of_tune = 0;        // will store last time when tuner was changed
uint8_t last_maker_pos=0;
uint8_t last_active_channel=0;
uint8_t first_channel_marker=1;
uint8_t update_frequency_view=0;
uint8_t seek_found=0;
uint8_t last_dip_channel=255;
uint8_t last_dip_band=255;
uint8_t scan_start=0;
uint8_t first_tune=1;
uint8_t force_menu_redraw=0;
uint16_t rssi_best=RSSI_MIN_VAL; // used for band scaner
uint16_t rssi_min_a=0;
uint16_t rssi_max_a=0;
uint16_t rssi_setup_min_a=0;
uint16_t rssi_setup_max_a=0;
#ifdef USE_DIVERSITY
uint16_t rssi_min_b=0;
uint16_t rssi_max_b=0;
uint16_t rssi_setup_min_b=0;
uint16_t rssi_setup_max_b=0;
#endif
uint16_t rssi_seek_found=0;
uint16_t rssi_setup_run=0;

//TVout TV;

// SETUP ----------------------------------------------------------------------------
void setup()
{
    // Set the address of your OLED Display.
    // 128x64 ONLY!!
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    // init done
    display.clearDisplay();   // clears the screen and buffer
    display.display(); // show splashscreen
    // IO INIT
    // initialize digital pin 13 LED as an output.
    pinMode(led, OUTPUT); // status pin for TV mode errors
    // buzzer
    pinMode(buzzer, OUTPUT); // Feedback buzzer (active buzzer, not passive piezo)
    digitalWrite(buzzer, HIGH);
    // minimum control pins
    pinMode(buttonSeek, INPUT);
    digitalWrite(buttonSeek, INPUT_PULLUP);
    pinMode(buttonMode, INPUT);
    digitalWrite(buttonMode, INPUT_PULLUP);
    // optional control
    pinMode(buttonDown, INPUT);
    digitalWrite(buttonDown, INPUT_PULLUP);
    pinMode(buttonSave, INPUT);
    digitalWrite(buttonSave, INPUT_PULLUP);
    //Receiver Setup
    pinMode(receiverA_led,OUTPUT);
    digitalWrite(buzzer, HIGH);
#ifdef USE_DIVERSITY
    pinMode(receiverB_led,OUTPUT);
    digitalWrite(receiverA_led, LOW);
#endif
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println(F("START:"));
#endif
    // SPI pins for RX control
    pinMode (slaveSelectPin, OUTPUT);
    pinMode (spiDataPin, OUTPUT);
	pinMode (spiClockPin, OUTPUT);
    // tune to first channel


    // init TV system
    char retVal = 0;
    // 0 if no error.
    // 1 if x is not divisable by 8.
    // 2 if y is to large (NTSC only cannot fill PAL vertical resolution by 8bit limit)
    // 4 if there is not enough memory for the frame buffer.
    if (retVal > 0) {
        // on Error flicker LED
        while (true) { // stay in ERROR for ever
            digitalWrite(led, !digitalRead(led));
            delay(100);
        }
    }
    //TV.select_font(font4x6);
    // Setup Done - LED ON
    digitalWrite(led, HIGH);

    // use values only of EEprom is not 255 = unsaved
    uint8_t eeprom_check = EEPROM.read(EEPROM_ADR_STATE);
    if(eeprom_check == 255) // unused
    {
        EEPROM.write(EEPROM_ADR_STATE,START_STATE);
        EEPROM.write(EEPROM_ADR_TUNE,CHANNEL_MIN_INDEX);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,highByte(RSSI_MAX_VAL));
#ifdef USE_DIVERSITY
        // diversity
        EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,highByte(RSSI_MAX_VAL));
#endif
    }

    // read last setting from eeprom
    state=EEPROM.read(EEPROM_ADR_STATE);
    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);
    rssi_min_a=((EEPROM.read(EEPROM_ADR_RSSI_MIN_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_A_L)));
    rssi_max_a=((EEPROM.read(EEPROM_ADR_RSSI_MAX_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_A_L)));
#ifdef USE_DIVERSITY
    diversity_mode = EEPROM.read(EEPROM_ADR_DIVERSITY);
    rssi_min_b=((EEPROM.read(EEPROM_ADR_RSSI_MIN_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_B_L)));
    rssi_max_b=((EEPROM.read(EEPROM_ADR_RSSI_MAX_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_B_L)));
#endif
    force_menu_redraw=1;
}

// LOOP ----------------------------------------------------------------------------
void loop()
{
    /*******************/
    /*   Mode Select   */
    /*******************/
    state_last_used=state; // save save settings
    if (digitalRead(buttonMode) == LOW) // key pressed ?
    {
        beep(50); // beep & debounce
        delay(KEY_DEBOUNCE/2); // debounce
        beep(50); // beep & debounce
        delay(KEY_DEBOUNCE/2); // debounce

        uint8_t press_time=0;
        // on entry wait for release
        while(digitalRead(buttonMode) == LOW)
        {
            delay(10);
            press_time++;
        }
        #define MAX_MENU 4
        #define MENU_Y_SIZE 15

        char menu_id=state_last_used-1;
        // Show Mode Screen
        if(state==STATE_SEEK_FOUND)
        {
            state=STATE_SEEK;
        }
        uint8_t in_menu=1;
        uint8_t in_menu_time_out=50; // 20x 100ms = 5 seconds
        /*
        Enter Mode menu
        Show current mode
        Change mode by MODE key
        Any Mode will refresh screen
        If not MODE changes in 2 seconds, it uses last used mode
        */
        do
        {
            if(press_time >= 10) // if menu held for 1 second invoke quick save.
            {
                // user held the mode button and wants to quick save.
                in_menu=0; // EXIT
                state = STATE_SAVE;
                break;
            }
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(BLACK);
            display.drawRect(0, 0, display.width(), display.height(), WHITE);
            display.fillRect(0, 0, display.width(), 11, WHITE);
            display.setCursor(22,2);
            display.print("MODE SELECTION");
            // selection by inverted box
            switch (menu_id)
            {
                case 0: // auto search
                    state=STATE_SEEK;
                    force_seek=1;
                    seek_found=0;
                break;
                case 1: // Band Scanner
                    state=STATE_SCAN;
                    scan_start=1;
                break;
                case 2: // manual mode
                    state=STATE_MANUAL;
                break;
            #ifdef USE_DIVERSITY
                case 3: // Diversity
                    state=STATE_DIVERSITY;
                break;
            #else
                case 3: // Skip
                    menu_id++;
            #endif
                case 4: // Save settings
                    state=STATE_SAVE;
                break;
            } // end switch

            // draw selected
            display.fillRect(0, 10*menu_id+12, display.width(), 10, WHITE);

            display.setTextSize(1);
            display.setTextColor(menu_id == 0 ? BLACK : WHITE);
            display.setCursor(5,10*0+13);
            display.print("AUTO SEARCH");
            display.setTextColor(menu_id == 1 ? BLACK : WHITE);
            display.setCursor(5,10*1+13);
            display.print("BAND SCANNER");
            display.setTextColor(menu_id == 2 ? BLACK : WHITE);
            display.setCursor(5,10*2+13);
            display.print("MANUAL MODE");
#ifdef USE_DIVERSITY
            display.setTextColor(menu_id == 3 ? BLACK : WHITE);
            display.setCursor(5,10*3+13);
            display.print("DIVERSITY");
#endif
            display.setTextColor(menu_id == 4 ? BLACK : WHITE);
            display.setCursor(5,10*4+13);
            display.println("SAVE SETUP");
            display.display();

            while(digitalRead(buttonMode) == LOW || digitalRead(buttonSeek) == LOW || digitalRead(buttonDown) == LOW)
            {
                // wait for MODE release
                in_menu_time_out=50;
            }
            while(--in_menu_time_out && ((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonSeek) == HIGH) && (digitalRead(buttonDown) == HIGH))) // wait for next key press or time out
            {
                delay(100); // timeout delay
            }
            if(in_menu_time_out==0 || digitalRead(buttonMode) == LOW)
            {
                if(digitalRead(buttonMode) != LOW) {
                    state=state_last_used; // exit to last state on timeout.
                }
                in_menu=0; // EXIT
                beep(KEY_DEBOUNCE/2); // beep & debounce
                delay(50); // debounce
                beep(KEY_DEBOUNCE/2); // beep & debounce
                delay(50); // debounce
            }
            else // no timeout, must be keypressed
            {
                /*********************/
                /*   Menu handler   */
                /*********************/
                if(digitalRead(buttonSeek) == LOW) {
                    menu_id++;
                }
                else if(digitalRead(buttonDown) == LOW) {
                    menu_id--;
                }

                if (menu_id > MAX_MENU)
                {
                    menu_id = 0; // next state
                }
                if(menu_id < 0)
                {
                    menu_id = MAX_MENU;
                }
                in_menu_time_out=50;
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
            }
        } while(in_menu);
        last_state=255; // force redraw of current screen
        switch_count = 0;
    }
    else // key pressed
    { // reset debounce
        switch_count = 0;
    }
    /***********************/
    /*     Save buttom     */
    /***********************/
    // hardware save buttom support (if no display is used)
    if(digitalRead(buttonSave) == LOW)
    {
        state=STATE_SAVE;
    }
    /***************************************/
    /*   Draw screen if mode has changed   */
    /***************************************/
    if(force_menu_redraw || state != last_state)
    {
        force_menu_redraw=0;
        /************************/
        /*   Main screen draw   */
        /************************/
        // changed state, clear an draw new screen

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        // simple menu
        #define TV_Y_GRID 14
        #define TV_Y_OFFSET 3
        switch (state)
        {
            case STATE_SCAN: // Band Scanner
            case STATE_RSSI_SETUP: // RSSI setup

                // draw selected
                display.drawRect(0, 0, display.width(), display.height(), WHITE);
                display.fillRect(0, 0, display.width(), 11, WHITE);
                display.drawRect(0, 10, display.width(), 11, WHITE);

                if(state==STATE_SCAN)
                {
                    display.setTextColor(BLACK);
                    display.setCursor(30,2);
                    display.print("BAND SCANNER");
                    display.setTextColor(WHITE);
                    display.setCursor(5,12);
                    display.print("BEST:");
                }
                else
                {

                    display.setTextColor(BLACK);
                    display.setCursor(40,2);
                    display.print("RSSI SETUP");
                    display.setTextColor(WHITE);
                    display.setCursor(5,12);
                    display.print("Min:     Max:");
                    // prepare new setup
                    rssi_min_a=0;
                    rssi_max_a=400; // set to max range
                    rssi_setup_min_a=400;
                    rssi_setup_max_a=0;
#ifdef USE_DIVERSITY
                    rssi_min_b=0;
                    rssi_max_b=400; // set to max range
                    rssi_setup_min_b=400;
                    rssi_setup_max_b=0;
#endif
                    rssi_setup_run=RSSI_SETUP_RUN;
                }

                display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
                display.setCursor(2,display.height()-9);
                display.print("5645");
                display.setCursor(55,display.height()-9);
                display.print("5800");
                display.setCursor(display.width()-25,display.height()-9);
                display.print("5945");
                // trigger new scan from begin
                channel=CHANNEL_MIN;
                channelIndex = pgm_read_byte_near(channelList + channel);
                rssi_best=RSSI_MIN_VAL;
                scan_start=1;
                display.display();
            break;
            case STATE_MANUAL: // manual mode
            case STATE_SEEK: // seek mode
                display.drawRect(0, 0, display.width(), display.height(), WHITE);
                display.fillRect(0, 0, display.width(), 11, WHITE);
                display.drawRect(0, 10, display.width(), 11, WHITE);
                if (state == STATE_MANUAL)
                {
                    display.setTextColor(BLACK);
                    display.setCursor(35,2);
                    display.print("MANUAL MODE");
                }
                else if(state == STATE_SEEK)
                {
                    display.setCursor(25,2);
                    display.setTextColor(BLACK);
                    display.print("AUTO SEEK MODE");
                }
                display.setTextColor(WHITE);

                display.drawLine(0, 32, display.width(), 32, WHITE);
                display.setCursor(5,12);
                display.drawLine(97,11,97,20,WHITE);
                display.print("BAND:");
                for(int i=0;i<8;i++) {
                    display.setCursor(15*i+8,23);
                    display.print((char) (i+'1'));
                }
                display.drawLine(0, 36, display.width(), 36, WHITE);


                display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
                display.setCursor(2,display.height()-9);
                display.print("5645");
                display.setCursor(55,display.height()-9);
                display.print("5800");
                display.setCursor(display.width()-25,display.height()-9);
                display.print("5945");
                first_channel_marker=1;
                update_frequency_view=1;
                force_seek=1;
            break;
#ifdef USE_DIVERSITY
            case STATE_DIVERSITY:
                // diversity menu is below this is just a place holder.
            break;
#endif
            case STATE_SAVE:
                EEPROM.write(EEPROM_ADR_TUNE,channelIndex);
                EEPROM.write(EEPROM_ADR_STATE,state_last_used);
#ifdef USE_DIVERSITY
                EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
#endif

                display.drawRect(0, 0, display.width(), display.height(), WHITE);
                display.fillRect(0, 0, display.width(), 11, WHITE);
                display.drawLine(0, 10, display.width(), 10, WHITE);
                display.setTextColor(BLACK);
                display.setCursor(35,2);
                display.print("SAVE SETTINGS");
                display.setTextColor(WHITE);
                display.setCursor(5,8*1+4);
                display.print("MODE:");
                display.setCursor(38,8*1+4);
                switch (state_last_used)
                {
                    case STATE_SCAN: // Band Scanner
                        display.print("BAND SCANNER");
                    break;
                    case STATE_MANUAL: // manual mode
                        display.print("MANUAL");
                    break;
                    case STATE_SEEK: // seek mode
                        display.print("AUTO SEEK");
                    break;
                }

                display.setCursor(5,8*2+4);
                display.print("BAND:");
                display.setCursor(38,8*2+4);
                // print band
                if(channelIndex > 31)
                {
                    display.print("C/Race");
                }
                else if(channelIndex > 23)
                {
                    display.print("F/Airwave");
                }
                else if (channelIndex > 15)
                {
                    display.print("E");
                }
                else if (channelIndex > 7)
                {
                    display.print("B");
                }
                else
                {
                    display.print("A");
                }

                display.setCursor(5,8*3+4);
                display.print("CHAN:");
                display.setCursor(38,8*3+4);
                uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE+1; // get channel inside band
                display.print(active_channel,DEC);
                display.setCursor(5,8*4+4);
                display.print("FREQ:     GHz");
                display.setCursor(38,8*4+4);
                display.print(pgm_read_word_near(channelFreqTable + channelIndex));
                display.setCursor(35,8*5+4);
                display.print("-- SAVED --");
                display.display();
                uint8_t loop=0;
                for (loop=0;loop<5;loop++)
                {
                    beep(100); // beep
                    delay(100);
                }
                delay(1000);
                display.setCursor(5,8*6+4);
                display.print("HOLD MODE RSSI SETUP");
                display.display();
                delay(1000);
                delay(1000);
                if (digitalRead(buttonMode) == LOW) // to RSSI setup
                {
                    display.setCursor(5,8*6+4);

                    display.setTextColor(WHITE,BLACK);
                    display.print("ENTERING RSSI SETUP ");
                    display.display();
                    uint8_t loop=0;
                    for (loop=0;loop<10;loop++)
                    {
                        #define RSSI_SETUP_BEEP 25
                        beep(RSSI_SETUP_BEEP); // beep & debounce
                        delay(RSSI_SETUP_BEEP); // debounce
                    }
                    state=STATE_RSSI_SETUP;
                    while(digitalRead(buttonMode) == LOW)
                    {
                        // wait for release
                    }
                    delay(KEY_DEBOUNCE);  // debounce
                }
                else
                {
                    delay(1000);
                    state=state_last_used; // return to saved function
                }
                force_menu_redraw=1; // we change the state twice, must force redraw of menu

            // selection by inverted box
            break;
        } // end switch

        display.display();
        last_state=state;
    }
    /*************************************/
    /*   Processing depending of state   */
    /*************************************/

#ifdef USE_DIVERSITY
    if(state == STATE_DIVERSITY) {
        // simple menu
        char menu_id=diversity_mode;
        uint8_t in_menu=1;
        do{
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.drawRect(0, 0, display.width(), display.height(), WHITE);
            display.fillRect(0, 0, display.width(), 11, WHITE);
            display.drawLine(0, 10, display.width(), 10, WHITE);
            display.setTextColor(BLACK);
            display.setCursor(40,2);
            display.print("DIVERSITY");

            //selected
            display.fillRect(0, 10*menu_id+12, display.width(), 11, WHITE);

            display.setTextColor(menu_id == useReceiverAuto ? BLACK : WHITE);
            display.setCursor(5,11*1+2);
            display.print("AUTO");

            display.setTextColor(menu_id == useReceiverA ? BLACK : WHITE);
            display.setCursor(5,11*2+2);
            display.print("RECEIVER A");
            display.setTextColor(menu_id == useReceiverB ? BLACK : WHITE);
            display.setCursor(5,11*3+2);
            display.print("RECEIVER B");

            // RSSI Strength
            display.setTextColor(WHITE);
            display.drawRect(0, display.height()-21, display.width(), 11, WHITE);
            display.setCursor(5,display.height()-19);
            display.print("A:");
            display.setCursor(5,display.height()-9);
            display.print("B:");
            display.display();
            switch (menu_id)
            {
                case useReceiverAuto:
                    break;
                case useReceiverA:
                    digitalWrite(receiverA_led, HIGH);
                    digitalWrite(receiverB_led, LOW);
                    break;
                case useReceiverB:
                    digitalWrite(receiverA_led, LOW);
                    digitalWrite(receiverB_led, HIGH);
                    break;
            }
            do
            {
                delay(10); // timeout delay
                // show signal strength
                wait_rssi_ready();
                if(menu_id == useReceiverAuto) {
                    readRSSI(); // update LED
                }
                // read rssi A
                rssi = readRSSI(useReceiverA);
                #define RSSI_BAR_SIZE 108
                rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);

                display.fillRect(18 + rssi_scaled, display.height()-19, (RSSI_BAR_SIZE-rssi_scaled), 7, BLACK);
                display.fillRect(18, display.height()-19, rssi_scaled, 7, WHITE);


                // read rssi B
                rssi = readRSSI(useReceiverB);
                rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
                display.fillRect(18 + rssi_scaled, display.height()-20, (RSSI_BAR_SIZE-rssi_scaled), 7, BLACK);
                display.fillRect(18, display.height()-9, rssi_scaled, 7, WHITE);
                display.display();
            }
            while((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonSeek) == HIGH) && (digitalRead(buttonDown) == HIGH)); // wait for next mode or time out

            if(digitalRead(buttonMode) == LOW)        // channel UP
            {
                diversity_mode = menu_id;
                in_menu = 0; // exit menu
            }
            else if(digitalRead(buttonSeek) == LOW) {
                menu_id++;
            }
            else if(digitalRead(buttonDown) == LOW) {
                menu_id--;
            }

            if(menu_id > useReceiverB) {
                menu_id = 0;
            }
            if(menu_id < 0) {
                menu_id = useReceiverB;
            }
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
        }
        while(in_menu);

        state=state_last_used;
    }
#endif
    /*****************************************/
    /*   Processing MANUAL MODE / SEEK MODE  */
    /*****************************************/
    if(state == STATE_MANUAL || state == STATE_SEEK)
    {
        if(state == STATE_MANUAL) // MANUAL MODE
        {
            // handling of keys
            if( digitalRead(buttonSeek) == LOW)        // channel UP
            {
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
                channelIndex++;
                if (channelIndex > CHANNEL_MAX_INDEX)
                {
                    channelIndex = CHANNEL_MIN_INDEX;
                }
                update_frequency_view=1;
            }
            if( digitalRead(buttonDown) == LOW) // channel DOWN
            {
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
                channelIndex--;
                if (channelIndex > CHANNEL_MAX_INDEX) // negative overflow
                {
                    channelIndex = CHANNEL_MAX_INDEX;
                }
                update_frequency_view=1;
            }
        }
        // display refresh handler
        if(update_frequency_view) // only updated on changes
        {
            display.setTextColor(WHITE,BLACK);
            display.setCursor(36,12);
            // show current used channel of bank
            if(channelIndex > 31)
            {
                display.print("C/Race   ");
            }
            else if(channelIndex > 23)
            {
                display.print("F/Airwave");
            }
            else if (channelIndex > 15)
            {
                display.print("E        ");
            }
            else if (channelIndex > 7)
            {
                display.print("B        ");
            }
            else
            {
                display.print("A        ");
            }


            uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE; // get channel inside band
            for(int i=0;i<8;i++) {
                display.fillRect(15*i+4,21,14,11,i==active_channel? WHITE:BLACK);
                display.setTextColor(i==active_channel? BLACK:WHITE);
                display.setCursor(15*i+8,23);
                display.print((char) (i+'1'));
            }

            first_channel_marker=0;
            last_active_channel=active_channel;
            // show frequence
            display.setCursor(101,12);
            display.setTextColor(WHITE,BLACK);
            display.print(pgm_read_word_near(channelFreqTable + channelIndex));
        }
        // show signal strength
        wait_rssi_ready();
        rssi = readRSSI();
        rssi_scaled=map(rssi, 1, 100, 1, display.width()-3);

        display.fillRect(1+rssi_scaled, 33, display.width()-3-rssi_scaled, 3, BLACK);
        display.fillRect(1, 33, rssi_scaled, 3, WHITE);

        channel=channel_from_index(channelIndex); // get 0...40 index depending of current channel
        rssi_scaled=map(rssi, 1, 100, 1, 14);

        hight = (display.height()-12-rssi_scaled);
        display.fillRect((channel*3)+4,display.height()-12-14,3,14-rssi_scaled,BLACK);
        display.fillRect((channel*3)+4,hight,3,rssi_scaled,WHITE);
        if(channel < CHANNEL_MAX_INDEX)
        {
            last_maker_pos=channel;
        }
        else
        {
          //  No action on last position to keep frame intact
        }
        // handling for seek mode after screen and RSSI has been fully processed
        if(state == STATE_SEEK) //
        { // SEEK MODE
            if(!seek_found) // search if not found
            {
                if ((!force_seek) && (rssi > RSSI_SEEK_TRESHOLD)) // check for found channel
                {
                    seek_found=1;
                    // beep twice as notice of lock
                    beep(100);
                    delay(100);
                    beep(100);
                }
                else
                { // seeking itself
                    force_seek=0;
                    // next channel
                    if (channel < CHANNEL_MAX)
                    {
                        channel++;
                    } else {
                        channel=CHANNEL_MIN;
                    }
                    channelIndex = pgm_read_byte_near(channelList + channel);
                }
            }
            else
            { // seek was successful
                display.setTextColor(BLACK,WHITE);
                display.setCursor(25,2);
                display.print("AUTO MODE LOCK");
                if (digitalRead(buttonSeek) == LOW) // restart seek if key pressed
                {
                    beep(50); // beep & debounce
                    delay(KEY_DEBOUNCE); // debounce
                    force_seek=1;
                    seek_found=0;

                    display.setCursor(25,2);
                    display.print("AUTO SEEK MODE");
                }
            }
        }
        display.display();
    }
    /****************************/
    /*   Processing SCAN MODE   */
    /****************************/
    else if (state == STATE_SCAN || state == STATE_RSSI_SETUP)
    {
        // force tune on new scan start to get right RSSI value
        if(scan_start)
        {
            scan_start=0;
            setChannelModule(channelIndex);
            last_channel_index=channelIndex;
            // keep time of tune to make sure that RSSI is stable when required
            time_of_tune=millis();
        }
        // channel marker
        if(channel < CHANNEL_MAX_INDEX)
        {
            last_maker_pos=channel;
        }
        else
        {
          //  No action on last position to keep frame intact
        }
        // print bar for spectrum
        wait_rssi_ready();
        // value must be ready
        rssi = readRSSI();
        rssi_scaled=map(rssi, 1, 100, 1, 30);
        hight = (display.height()-12-rssi_scaled);
        display.fillRect((channel*3)+4,display.height()-12-30,3,30-rssi_scaled,BLACK);
        display.fillRect((channel*3)+4,hight,3,rssi_scaled,WHITE);
        if(state == STATE_SCAN)
        {
            if (rssi > RSSI_SEEK_TRESHOLD)
            {
                if(rssi_best < rssi) {
                    rssi_best = rssi;
                    display.setTextColor(WHITE,BLACK);
                    display.setCursor(36,12);
                    display.print(pgm_read_byte_near(channelNames + channelIndex), HEX);
                    display.setCursor(52,12);
                    display.print(pgm_read_word_near(channelFreqTable + channelIndex));

                }
            }
        }
        // next channel
        if (channel < CHANNEL_MAX)
        {
            channel++;
        }
        else
        {
            channel=CHANNEL_MIN;
            if(state == STATE_RSSI_SETUP)
            {
                if(!rssi_setup_run--)
                {
                    // setup done
                    rssi_min_a=rssi_setup_min_a;
                    rssi_max_a=rssi_setup_max_a;
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,(rssi_min_a & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,(rssi_min_a >> 8));
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,(rssi_max_a & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,(rssi_max_a >> 8));


#ifdef USE_DIVERSITY
                    rssi_min_b=rssi_setup_min_b;
                    rssi_max_b=rssi_setup_max_b;
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,(rssi_min_b & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,(rssi_min_b >> 8));
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,(rssi_max_b & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,(rssi_max_b >> 8));
#endif
                    state=EEPROM.read(EEPROM_ADR_STATE);
                    beep(1000);
                }
            }
        }
        // new scan possible by press scan
        if (digitalRead(buttonSeek) == LOW) // force new full new scan
        {
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            last_state=255; // force redraw by fake state change ;-)
            channel=CHANNEL_MIN;
            scan_start=1;
            rssi_best=RSSI_MIN_VAL;
        }
        // update index after channel change
        channelIndex = pgm_read_byte_near(channelList + channel);
        display.display();
    }

    /*****************************/
    /*   General house keeping   */
    /*****************************/
    if(last_channel_index != channelIndex)         // tune channel on demand
    {
        setChannelModule(channelIndex);
        last_channel_index=channelIndex;
        // keep time of tune to make sure that RSSI is stable when required
        time_of_tune=millis();
        // give 3 beeps when tuned to give feedback of correct start
        if(first_tune)
        {
            first_tune=0;
            #define UP_BEEP 100
            beep(UP_BEEP);
            delay(UP_BEEP);
            beep(UP_BEEP);
            delay(UP_BEEP);
            beep(UP_BEEP);
        }
    }
    //rssi = readRSSI();

}

/*###########################################################################*/
/*******************/
/*   SUB ROUTINES  */
/*******************/

void beep(uint16_t time)
{
    digitalWrite(led, LOW);
    digitalWrite(buzzer, LOW);
    delay(time);
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, HIGH);
}

uint8_t channel_from_index(uint8_t channelIndex)
{
    uint8_t loop=0;
    uint8_t channel=0;
    for (loop=0;loop<=CHANNEL_MAX;loop++)
    {
        if(pgm_read_byte_near(channelList + loop) == channelIndex)
        {
            channel=loop;
            break;
        }
    }
    return (channel);
}

void wait_rssi_ready()
{
    // CHECK FOR MINIMUM DELAY
    // check if RSSI is stable after tune by checking the time
    uint16_t tune_time = millis()-time_of_tune;
    // module need >20ms to tune.
    // 25 ms will to a 40 channel scan in 1 second.
    #define MIN_TUNE_TIME 25
    if(tune_time < MIN_TUNE_TIME)
    {
        // wait until tune time is full filled
        delay(MIN_TUNE_TIME-tune_time);
    }
}

uint16_t readRSSI()
{
#ifdef USE_DIVERSITY
    return readRSSI(-1);
}
uint16_t readRSSI(uint8_t receiver)
{
#endif
    uint16_t rssi = 0;
    uint16_t rssiA = 0;

#ifdef USE_DIVERSITY
    uint16_t rssiB = 0;
#endif
    for (uint8_t i = 0; i < 10; i++)
    {
        rssiA += analogRead(rssiPinA);

#ifdef USE_DIVERSITY
        rssiB += analogRead(rssiPinB);
#endif
    }
    rssiA = rssiA/10; // average of 10 readings

#ifdef USE_DIVERSITY
    rssiB = rssiB/10; // average of 10 readings
    // choosing which receiver RSSI to use.. do not change LED
    if(receiver == useReceiverA)
    {
        rssi=rssiA;
    }
    else if(receiver == useReceiverB)
    {
        rssi=rssiB;
    }
    else{
        switch(diversity_mode)
        {
            case useReceiverAuto:
                // select receiver
                if((int)abs((float)(((float)rssiA - (float)rssiB) / (float)rssiB) * 100.0) >= DIVERSITY_CUTOVER)
                {
                    if(rssiA > rssiB)
                    {
                        receiver=useReceiverA;
                        setReceiver(useReceiverA);
                    }
                    else
                    {
                        receiver=useReceiverB;
                        setReceiver(useReceiverB);
                    }
                }
                else {
                    if(digitalRead(receiverA_led) == HIGH) {
                        receiver=useReceiverA;
                        setReceiver(useReceiverA);
                    }
                    else {
                        receiver=useReceiverB;
                        setReceiver(useReceiverB);
                    }
                }
                break;
            case useReceiverA:
                receiver=useReceiverA;
                setReceiver(useReceiverA);
                break;
            case useReceiverB:
                receiver=useReceiverB;
                setReceiver(useReceiverB);
                break;
        }
    }
#else
rssi=rssiA;
#endif

    // special case for RSSI setup
    if(state==STATE_RSSI_SETUP)
    { // RSSI setup
        if(rssiA < rssi_setup_min_a)
        {
            rssi_setup_min_a=rssiA;

            display.setCursor(30,12);
            display.setTextColor(WHITE,BLACK);
            display.print( rssi_setup_min_a , DEC);
        }
        if(rssiA > rssi_setup_max_a)
        {
            rssi_setup_max_a=rssiA;

            display.setCursor(85,12);
            display.setTextColor(WHITE,BLACK);
            display.print( rssi_setup_max_a , DEC);
        }

#ifdef USE_DIVERSITY
        if(rssiB < rssi_setup_min_b)
        {
            rssi_setup_min_b=rssiB;
        }
        if(rssiB > rssi_setup_max_b)
        {
            rssi_setup_max_b=rssiB;
        }
#endif
    }
#ifdef USE_DIVERSITY
    if(receiver == useReceiverA || state==STATE_RSSI_SETUP)
    {
#endif
        rssi = constrain(rssiA, rssi_min_a, rssi_max_a);    //original 90---250
        rssi=rssi-rssi_min_a; // set zero point (value 0...160)
        rssi = map(rssi, 0, rssi_max_a-rssi_min_a , 1, 100);   // scale from 1..100%
#ifdef USE_DIVERSITY
    }
    else {
        rssi = constrain(rssiB, rssi_min_b, rssi_max_b);    //original 90---250
        rssi=rssi-rssi_min_b; // set zero point (value 0...160)
        rssi = map(rssi, 0, rssi_max_a-rssi_min_b , 1, 100);   // scale from 1..100%
    }
#endif
    return (rssi);
}
#ifdef USE_DIVERSITY
void setReceiver(uint8_t receiver) {
    if(receiver == useReceiverA)
    {
        digitalWrite(receiverA_led, HIGH);
        digitalWrite(receiverB_led, LOW);
    }
    else
    {
        digitalWrite(receiverB_led, HIGH);
        digitalWrite(receiverA_led, LOW);
    }
}
#endif

// Private function: from http://arduino.cc/playground/Code/AvailableMemory
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setChannelModule(uint8_t channel)
{
  uint8_t i;
  uint16_t channelData;

  //channelData = pgm_read_word(&channelTable[channel]);
  //channelData = channelTable[channel];
  channelData = pgm_read_word_near(channelTable + channel);

  // bit bash out 25 bits of data
  // Order: A0-3, !R/W, D0-D19
  // A0=0, A1=0, A2=0, A3=1, RW=0, D0-19=0
  SERIAL_ENABLE_HIGH();
  delayMicroseconds(1);
  //delay(2);
  SERIAL_ENABLE_LOW();

  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT1();

  SERIAL_SENDBIT0();

  // remaining zeros
  for (i = 20; i > 0; i--)
    SERIAL_SENDBIT0();

  // Clock the data in
  SERIAL_ENABLE_HIGH();
  //delay(2);
  delayMicroseconds(1);
  SERIAL_ENABLE_LOW();

  // Second is the channel data from the lookup table
  // 20 bytes of register data are sent, but the MSB 4 bits are zeros
  // register address = 0x1, write, data0-15=channelData data15-19=0x0
  SERIAL_ENABLE_HIGH();
  SERIAL_ENABLE_LOW();

  // Register 0x1
  SERIAL_SENDBIT1();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();

  // Write to register
  SERIAL_SENDBIT1();

  // D0-D15
  //   note: loop runs backwards as more efficent on AVR
  for (i = 16; i > 0; i--)
  {
    // Is bit high or low?
    if (channelData & 0x1)
    {
      SERIAL_SENDBIT1();
    }
    else
    {
      SERIAL_SENDBIT0();
    }

    // Shift bits along to check the next one
    channelData >>= 1;
  }

  // Remaining D16-D19
  for (i = 4; i > 0; i--)
    SERIAL_SENDBIT0();

  // Finished clocking data in
  SERIAL_ENABLE_HIGH();
  delayMicroseconds(1);
  //delay(2);

  digitalWrite(slaveSelectPin, LOW);
  digitalWrite(spiClockPin, LOW);
  digitalWrite(spiDataPin, LOW);
}


void SERIAL_SENDBIT1()
{
  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);

  digitalWrite(spiDataPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(spiClockPin, HIGH);
  delayMicroseconds(1);

  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);
}

void SERIAL_SENDBIT0()
{
  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);

  digitalWrite(spiDataPin, LOW);
  delayMicroseconds(1);
  digitalWrite(spiClockPin, HIGH);
  delayMicroseconds(1);

  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);
}

void SERIAL_ENABLE_LOW()
{
  delayMicroseconds(1);
  digitalWrite(slaveSelectPin, LOW);
  delayMicroseconds(1);
}

void SERIAL_ENABLE_HIGH()
{
  delayMicroseconds(1);
  digitalWrite(slaveSelectPin, HIGH);
  delayMicroseconds(1);
}
