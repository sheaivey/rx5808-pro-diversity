/*
 * SPI driver based on fs_skyrf_58g-main.c Written by Simon Chambers
 * TVOUT by Myles Metzel
 * Scanner by Johan Hermen
 * Inital 2 Button version by Peter (pete1990)
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

#include <TVout.h>
#include <fontALL.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

// Feature Togglels
//#define DEBUG

#define spiDataPin 10
#define slaveSelectPin 11
#define spiClockPin 12

// Receiver PINS
#define receiverA_led A1
#define rssiPinA A7

// Diversity
#define receiverB_led A0
#define rssiPinB A6
#define useReceiverAuto 0
#define useReceiverA 1
#define useReceiverB 2
// rssi strenth should be 2% greater than other receiver before switch.
// this pervents flicker when rssi are close.
#define DIVERSITY_CUTOVER 2

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

// Set you TV format (PAL = Europe = 50Hz, NTSC = INT = 60Hz)
//#define TV_FORMAT NTSC
#define TV_FORMAT PAL

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
#define STATE_DIVERSITY 4
#define STATE_SAVE 5
#define STATE_RSSI_SETUP 6

#define START_STATE STATE_SEEK
#define MAX_STATE STATE_MANUAL

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN_INDEX 0
#define CHANNEL_MAX_INDEX 39

#define CHANNEL_MAX 39
#define CHANNEL_MIN 0

#define TV_COLS 128
#define TV_ROWS 96
#define TV_Y_MAX TV_ROWS-1
#define TV_X_MAX TV_COLS-1
#define TV_SCANNER_OFFSET 14
#define SCANNER_BAR_SIZE 52
#define SCANNER_LIST_X_POS 4
#define SCANNER_LIST_Y_POS 16
#define SCANNER_MARKER_SIZE 2

#define EEPROM_ADR_STATE 0
#define EEPROM_ADR_TUNE 1
#define EEPROM_ADR_RSSI_MIN_A_L 2
#define EEPROM_ADR_RSSI_MIN_A_H 3
#define EEPROM_ADR_RSSI_MAX_A_L 4
#define EEPROM_ADR_RSSI_MAX_A_H 5
#define EEPROM_ADR_DIVERSITY 6
#define EEPROM_ADR_RSSI_MIN_B_L 7
#define EEPROM_ADR_RSSI_MIN_B_H 8
#define EEPROM_ADR_RSSI_MAX_B_L 9
#define EEPROM_ADR_RSSI_MAX_B_H 10


// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
  0x2A05,    0x299B,    0x2991,    0x2987,    0x291D,    0x2913,    0x2909,    0x289F,    // Band A
  0x2903,    0x290C,    0x2916,    0x291F,    0x2989,    0x2992,    0x299C,    0x2A05,    // Band B
  0x2895,    0x288B,    0x2881,    0x2817,    0x2A0F,    0x2A19,    0x2A83,    0x2A8D,    // Band E
  0x2906,    0x2910,    0x291A,    0x2984,    0x298E,    0x2998,    0x2A02,    0x2A0C,    // Band F / Airwave
  0x281D,    0x288F,    0x2902,    0x2914,    0x2978,    0x2999,    0x2A0C,    0x2A1E     // Band R / Immersion Raceband
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band R / Immersion Raceband
};

// do coding as simple hex value to save memory.
const uint8_t channelNames[] PROGMEM = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8,
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8,
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8
};

// All Channels of the above List ordered by Mhz
const uint8_t channelList[] PROGMEM = {
  19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
};

uint8_t channel = 0;
uint8_t channelIndex = 0;
uint8_t rssi = 0;
uint8_t rssi_scaled = 0;
uint8_t diversity_mode = useReceiverAuto;
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
uint16_t rssi_min_a=0;
uint16_t rssi_max_a=0;
uint16_t rssi_setup_min_a=0;
uint16_t rssi_setup_max_a=0;
uint16_t rssi_min_b=0;
uint16_t rssi_max_b=0;
uint16_t rssi_setup_min_b=0;
uint16_t rssi_setup_max_b=0;
uint16_t rssi_seek_found=0;
uint16_t rssi_setup_run=0;

TVout TV;

// SETUP ----------------------------------------------------------------------------
void setup()
{
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
    pinMode(receiverB_led,OUTPUT);
    digitalWrite(receiverA_led, LOW);
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
    char retVal = TV.begin(TV_FORMAT, TV_COLS, TV_ROWS);
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
    TV.select_font(font4x6);
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
        // diversity
        EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,highByte(RSSI_MAX_VAL));
    }

    // read last setting from eeprom
    state=EEPROM.read(EEPROM_ADR_STATE);
    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);
    rssi_min_a=((EEPROM.read(EEPROM_ADR_RSSI_MIN_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_A_L)));
    rssi_max_a=((EEPROM.read(EEPROM_ADR_RSSI_MAX_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_A_L)));
    diversity_mode = EEPROM.read(EEPROM_ADR_DIVERSITY);
    rssi_min_b=((EEPROM.read(EEPROM_ADR_RSSI_MIN_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_B_L)));
    rssi_max_b=((EEPROM.read(EEPROM_ADR_RSSI_MAX_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_B_L)));
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
            TV.clear_screen();
            // simple menu
            TV.select_font(font8x8);
            TV.draw_rect(0,0,127,95,  WHITE);
            TV.draw_line(0,14,127,14,WHITE);
            TV.printPGM(10, 3,  PSTR("MODE SELECTION"));
            TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Auto Search"));
            TV.printPGM(10, 5+2*MENU_Y_SIZE, PSTR("Band Scanner"));
            TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("Manual Mode"));
            TV.printPGM(10, 5+4*MENU_Y_SIZE, PSTR("Diversity"));
            TV.printPGM(10, 5+5*MENU_Y_SIZE, PSTR("Save Setup"));
            // selection by inverted box
            switch (menu_id)
            {
                case 0: // auto search
                    TV.draw_rect(8,3+1*MENU_Y_SIZE,100,12,  WHITE, INVERT);
                    state=STATE_SEEK;
                    force_seek=1;
                    seek_found=0;
                break;
                case 1: // Band Scanner
                    TV.draw_rect(8,3+2*MENU_Y_SIZE,100,12,  WHITE, INVERT);
                    state=STATE_SCAN;
                    scan_start=1;
                break;
                case 2: // manual mode
                    TV.draw_rect(8,3+3*MENU_Y_SIZE,100,12,  WHITE, INVERT);
                    state=STATE_MANUAL;
                break;
                case 3: // Diversity
                    TV.draw_rect(8,3+4*MENU_Y_SIZE,100,12,  WHITE, INVERT);
                    state=STATE_DIVERSITY;
                break;
                case 4: // Save settings
                    TV.draw_rect(8,3+5*MENU_Y_SIZE,100,12,  WHITE, INVERT);
                    state=STATE_SAVE;
                break;
            } // end switch

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
        // clean line?
        TV.print(TV_COLS/2, (TV_ROWS/2), "             ");
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
        TV.clear_screen();
        // simple menu
        #define TV_Y_GRID 14
        #define TV_Y_OFFSET 3
        switch (state)
        {
            case STATE_SCAN: // Band Scanner
            case STATE_RSSI_SETUP: // RSSI setup
                TV.select_font(font8x8);
                TV.draw_rect(0,0,TV_X_MAX,1*TV_Y_GRID,  WHITE); // upper frame
                if(state==STATE_SCAN)
                {
                    TV.printPGM(10, TV_Y_OFFSET,  PSTR(" BAND SCANNER"));
                }
                else
                {
                    TV.printPGM(10, TV_Y_OFFSET,  PSTR("  RSSI SETUP "));
                    TV.select_font(font4x6);
                    TV.print(10, SCANNER_LIST_Y_POS, "RSSI Min:     RSSI Max:   ");
                    // prepare new setup
                    rssi_min_a=0;
                    rssi_max_a=400; // set to max range
                    rssi_setup_min_a=400;
                    rssi_setup_max_a=0;
                    rssi_setup_run=RSSI_SETUP_RUN;
                }
                TV.draw_rect(0,1*TV_Y_GRID,TV_X_MAX,9,  WHITE); // list frame
                TV.draw_rect(0,TV_ROWS - TV_SCANNER_OFFSET,TV_X_MAX,13,  WHITE); // lower frame
                TV.select_font(font4x6);
                TV.print(2, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5645");
                TV.print(57, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5800");
                TV.print(111, (TV_ROWS - TV_SCANNER_OFFSET + 2), "5945");
                // trigger new scan from begin
                channel=CHANNEL_MIN;
                writePos=SCANNER_LIST_X_POS; // reset channel list
                channelIndex = pgm_read_byte_near(channelList + channel);
                scan_start=1;
            break;
            case STATE_MANUAL: // manual mode
            case STATE_SEEK: // seek mode
                TV.select_font(font8x8);
                TV.draw_rect(0,0,TV_X_MAX,TV_Y_MAX,  WHITE); // outer frame
                if (state == STATE_MANUAL)
                {
                    TV.printPGM(10, TV_Y_OFFSET,  PSTR(" MANUAL MODE"));
                }
                else if(state == STATE_SEEK)
                {
                    TV.printPGM(10, TV_Y_OFFSET,  PSTR("AUTO MODE SEEK"));
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
                TV.select_font(font8x8);
                first_channel_marker=1;
                update_frequency_view=1;
                force_seek=1;
            break;
            case STATE_DIVERSITY:
                // diversity menu is below this is just a place holder.
            break;
            case STATE_SAVE:
                EEPROM.write(EEPROM_ADR_TUNE,channelIndex);
                EEPROM.write(EEPROM_ADR_STATE,state_last_used);
                EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
                TV.select_font(font8x8);
                TV.draw_rect(0,0,127,95,  WHITE);
                TV.draw_line(0,14,127,14,WHITE);
                TV.printPGM(10, 3,  PSTR("SAVE SETTINGS"));
                TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Mode:"));
                switch (state_last_used)
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
                TV.print(50,5+4*MENU_Y_SIZE, pgm_read_word_near(channelFreqTable + channelIndex));
                TV.printPGM(10, 5+5*MENU_Y_SIZE, PSTR("--- SAVED ---"));
                uint8_t loop=0;
                for (loop=0;loop<5;loop++)
                {
                    beep(100); // beep
                    delay(100);
                }
                delay(1000);
                TV.select_font(font4x6);
                TV.printPGM(10, 14+5*MENU_Y_SIZE,     PSTR("Hold MODE to enter RSSI setup"));
                delay(1000);
                delay(1000);
                if (digitalRead(buttonMode) == LOW) // to RSSI setup
                {
                    TV.printPGM(10, 14+5*MENU_Y_SIZE, PSTR("ENTERING RSSI SETUP ......   " ));
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
                    TV.printPGM(10, 14+5*MENU_Y_SIZE, PSTR("                             "));
                    delay(1000);
                    state=state_last_used; // return to saved function
                }
                    force_menu_redraw=1; // we change the state twice, must force redraw of menu

            // selection by inverted box
            break;
        } // end switch
        last_state=state;
    }
    /*************************************/
    /*   Processing depending of state   */
    /*************************************/
    if(state == STATE_DIVERSITY) {
        // simple menu
        char menu_id=diversity_mode;
        uint8_t in_menu=1;
        do{
            TV.clear_screen();
            TV.select_font(font8x8);
            TV.draw_rect(0,0,127,95,  WHITE);
            TV.draw_line(0,14,127,14,WHITE);
            TV.printPGM(10, 3,  PSTR("DIVERSITY"));
            TV.printPGM(10, 5+1*MENU_Y_SIZE, PSTR("Auto"));
            TV.printPGM(10, 5+2*MENU_Y_SIZE, PSTR("Receiver A"));
            TV.printPGM(10, 5+3*MENU_Y_SIZE, PSTR("Receiver B"));
            // RSSI Strength
            TV.draw_line(0,3+4*MENU_Y_SIZE, TV_X_MAX, 3+4*MENU_Y_SIZE, WHITE);
            TV.printPGM(10, 6+4*MENU_Y_SIZE, PSTR("A:"));
            TV.draw_line(0,3+5*MENU_Y_SIZE, TV_X_MAX, 3+5*MENU_Y_SIZE, WHITE);
            TV.printPGM(10, 6+5*MENU_Y_SIZE, PSTR("B:"));
            switch (menu_id)
            {
                case useReceiverAuto:
                    TV.draw_rect(8,3+1*MENU_Y_SIZE,100,12,  WHITE, INVERT); // auto
                    break;
                case useReceiverA:
                    TV.draw_rect(8,3+2*MENU_Y_SIZE,100,12,  WHITE, INVERT); // receiver a
                    digitalWrite(receiverA_led, HIGH);
                    digitalWrite(receiverB_led, LOW);
                    break;
                case useReceiverB:
                    TV.draw_rect(8,3+3*MENU_Y_SIZE,100,12,  WHITE, INVERT); // receiver b
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
                #define RSSI_BAR_SIZE 100
                rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
                // clear last bar
                TV.draw_rect(25+rssi_scaled, 6+4*MENU_Y_SIZE, RSSI_BAR_SIZE-rssi_scaled, 8 , BLACK, BLACK);
                //  draw new bar
                TV.draw_rect(25, 6+4*MENU_Y_SIZE, rssi_scaled, 8 , WHITE, WHITE);

                // read rssi B
                rssi = readRSSI(useReceiverB);
                rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
                // clear last bar
                TV.draw_rect(25+rssi_scaled, 6+5*MENU_Y_SIZE, RSSI_BAR_SIZE-rssi_scaled, 8 , BLACK, BLACK);
                //  draw new bar
                TV.draw_rect(25, 6+5*MENU_Y_SIZE, rssi_scaled, 8 , WHITE, WHITE);
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
            if(!first_channel_marker)
            {
                // clear last marker
                TV.draw_rect(last_active_channel*16+2 ,TV_Y_OFFSET-2+2*TV_Y_GRID,12,12,  BLACK, INVERT); // mark current channel
            }
            first_channel_marker=0;
            // set new marker
            TV.draw_rect(active_channel*16+2 ,TV_Y_OFFSET-2+2*TV_Y_GRID,12,12,  WHITE, INVERT); // mark current channel
            last_active_channel=active_channel;
            // show frequence
            TV.print(50,TV_Y_OFFSET+3*TV_Y_GRID, pgm_read_word_near(channelFreqTable + channelIndex));
        }
        // show signal strength
        wait_rssi_ready();
        rssi = readRSSI();
        #define RSSI_BAR_SIZE 100
        rssi_scaled=map(rssi, 1, 100, 1, RSSI_BAR_SIZE);
        // clear last bar
        TV.draw_rect(25, TV_Y_OFFSET+4*TV_Y_GRID, RSSI_BAR_SIZE,4 , BLACK, BLACK);
        //  draw new bar
        TV.draw_rect(25, TV_Y_OFFSET+4*TV_Y_GRID, rssi_scaled, 4 , WHITE, WHITE);
        // print bar for spectrum
        channel=channel_from_index(channelIndex); // get 0...31 index depending of current channel
        #define SCANNER_BAR_MINI_SIZE 14
        rssi_scaled=map(rssi, 1, 100, 1, SCANNER_BAR_MINI_SIZE);
        hight = (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled);
        // clear last bar
        TV.draw_rect((channel * 4), (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_MINI_SIZE), 3, SCANNER_BAR_MINI_SIZE , BLACK, BLACK);
        //  draw new bar
        TV.draw_rect((channel * 4), hight, 3, rssi_scaled , WHITE, WHITE);
        // set marker in spectrum to show current scanned channel
        if(channel < CHANNEL_MAX_INDEX)
        {
            // clear last square
            TV.draw_rect((last_maker_pos * 4)+2, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  BLACK, BLACK);
            // draw next
            TV.draw_rect((channel * 4)+2, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);
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
                TV.printPGM(10, TV_Y_OFFSET,  PSTR("AUTO MODE LOCK"));
                if (digitalRead(buttonSeek) == LOW) // restart seek if key pressed
                {
                    beep(50); // beep & debounce
                    delay(KEY_DEBOUNCE); // debounce
                    force_seek=1;
                    seek_found=0;
                    TV.printPGM(10, TV_Y_OFFSET,  PSTR("AUTO MODE SEEK"));
                }
            }
        }
        TV.delay_frame(1); // clean redraw
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
            // clear last square
            TV.draw_rect((last_maker_pos * 4)+2, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  BLACK, BLACK);
            // draw next
            TV.draw_rect((channel * 4)+2, (TV_ROWS - TV_SCANNER_OFFSET + 8),SCANNER_MARKER_SIZE,SCANNER_MARKER_SIZE,  WHITE, WHITE);
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
        rssi_scaled=map(rssi, 1, 100, 5, SCANNER_BAR_SIZE);
        hight = (TV_ROWS - TV_SCANNER_OFFSET - rssi_scaled);
        // clear last bar
        TV.draw_rect((channel * 3), (TV_ROWS - TV_SCANNER_OFFSET - SCANNER_BAR_SIZE), 2, SCANNER_BAR_SIZE , BLACK, BLACK);
        //  draw new bar
        TV.draw_rect((channel * 3), hight, 2, rssi_scaled , WHITE, WHITE);
        // print channelname
        if(state == STATE_SCAN)
        {
            if (rssi > RSSI_SEEK_TRESHOLD)
            {
                TV.draw_rect(writePos, SCANNER_LIST_Y_POS, 20, 6,  BLACK, BLACK);
                TV.print(writePos, SCANNER_LIST_Y_POS, pgm_read_byte_near(channelNames + channelIndex), HEX);
                TV.print(writePos+10, SCANNER_LIST_Y_POS, pgm_read_word_near(channelFreqTable + channelIndex));
                writePos += 30;
                // mark bar
                TV.print((channel * 3) - 3, hight - 5, pgm_read_byte_near(channelNames + channelIndex), HEX);
            }
        }
        // next channel
        if (channel < CHANNEL_MAX)
        {
            channel++;
        } else {
            channel=CHANNEL_MIN;
            writePos=SCANNER_LIST_X_POS; // reset channel list
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
                    rssi_min_b=rssi_setup_min_b;
                    rssi_max_b=rssi_setup_max_b;
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,(rssi_min_b & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,(rssi_min_b >> 8));
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,(rssi_max_b & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,(rssi_max_b >> 8));

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
            writePos=SCANNER_LIST_X_POS; // reset channel list
            scan_start=1;
        }
        // update index after channel change
        channelIndex = pgm_read_byte_near(channelList + channel);
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
    // 30 ms will to a 32 channel scan in 1 second.
    #define MIN_TUNE_TIME 30
    if(tune_time < MIN_TUNE_TIME)
    {
        // wait until tune time is full filled
        delay(MIN_TUNE_TIME-tune_time);
    }
}

uint16_t readRSSI()
{
    return readRSSI(-1);
}
uint16_t readRSSI(uint8_t receiver)
{
    uint16_t rssi = 0;
    uint16_t rssiA = 0;
    uint16_t rssiB = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        rssiA += analogRead(rssiPinA);
        rssiB += analogRead(rssiPinB);
    }
    rssiA = rssiA/10; // average of 10 readings
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


    // special case for RSSI setup
    if(state==STATE_RSSI_SETUP)
    { // RSSI setup
        if(rssiA < rssi_setup_min_a)
        {
            rssi_setup_min_a=rssiA;
            TV.print(50, SCANNER_LIST_Y_POS, "   ");
            TV.print(50, SCANNER_LIST_Y_POS, rssi_setup_min_a , DEC);
        }
        if(rssiA > rssi_setup_max_a)
        {
            rssi_setup_max_a=rssiA;
            TV.print(110, SCANNER_LIST_Y_POS, "   ");
            TV.print(110, SCANNER_LIST_Y_POS, rssi_setup_max_a , DEC);
        }
        if(rssiB < rssi_setup_min_b)
        {
            rssi_setup_min_b=rssiB;
        }
        if(rssiB > rssi_setup_max_b)
        {
            rssi_setup_max_b=rssiB;
        }
    }

    if(receiver == useReceiverA || state==STATE_RSSI_SETUP)
    {
        rssi = constrain(rssiA, rssi_min_a, rssi_max_a);    //original 90---250
        rssi=rssi-rssi_min_a; // set zero point (value 0...160)
        rssi = map(rssi, 0, rssi_max_a-rssi_min_a , 1, 100);   // scale from 1..100%
    }
    else {
        rssi = constrain(rssiB, rssi_min_b, rssi_max_b);    //original 90---250
        rssi=rssi-rssi_min_b; // set zero point (value 0...160)
        rssi = map(rssi, 0, rssi_max_a-rssi_min_b , 1, 100);   // scale from 1..100%
    }
    return (rssi);
}

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
