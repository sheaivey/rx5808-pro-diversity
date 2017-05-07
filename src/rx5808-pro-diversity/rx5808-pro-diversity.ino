/*
 * SPI driver based on fs_skyrf_58g-main.c Written by Simon Chambers
 * TVOUT by Myles Metzel
 * Scanner by Johan Hermen
 * Inital 2 Button version by Peter (pete1990)
 * Refactored and GUI reworked by Marko Hoepken
 * Universal version my Marko Hoepken
 * Diversity Receiver Mode and GUI improvements by Shea Ivey
 * OLED Version by Shea Ivey
 * Seperating display concerns by Shea Ivey

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

#include <avr/pgmspace.h>
#include <EEPROM.h>

#include "settings.h"

// uncomment depending on the display you are using.
// this is an issue with the arduino preprocessor
#ifdef TVOUT_SCREENS
//   #include <TVout.h>
//   #include <fontALL.h>
#endif
#ifdef OLED_128x64_ADAFRUIT_SCREENS

	#ifdef SH1106
		#include <Adafruit_SH1106.h>
	#else
		#include <Adafruit_SSD1306.h>
	#endif
    #include <Adafruit_GFX.h>
    #include <Wire.h>
    #include <SPI.h>
#endif

#include "screens.h"
screens drawScreen;

// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
  0x2A05,    0x299B,    0x2991,    0x2987,    0x291D,    0x2913,    0x2909,    0x289F,    // Band A
  0x2903,    0x290C,    0x2916,    0x291F,    0x2989,    0x2992,    0x299C,    0x2A05,    // Band B
  0x2895,    0x288B,    0x2881,    0x2817,    0x2A0F,    0x2A19,    0x2A83,    0x2A8D,    // Band E
  0x2906,    0x2910,    0x291A,    0x2984,    0x298E,    0x2998,    0x2A02,    0x2A0C,    // Band F / Airwave
#ifdef USE_LBAND
  0x281D,    0x288F,    0x2902,    0x2914,    0x2987,    0x2999,    0x2A0C,    0x2A1E,    // Band C / Immersion Raceband
  0x2609,    0x261C,    0x268E,    0x2701,    0x2713,    0x2786,    0x2798,    0x280B     // Band D / 5.3
#else
  0x281D,    0x288F,    0x2902,    0x2914,    0x2987,    0x2999,    0x2A0C,    0x2A1E     // Band C / Immersion Raceband
#endif
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
#ifdef USE_LBAND
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // Band C / Immersion Raceband
  5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621  // Band D / 5.3
#else
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band C / Immersion Raceband
#endif
};

// do coding as simple hex value to save memory.
const uint8_t channelNames[] PROGMEM = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // Band A
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, // Band B
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // Band E
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, // Band F / Airwave
#ifdef USE_LBAND
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, // Band C / Immersion Raceband
  0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8  // BAND D / 5.3
#else
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8  // Band C / Immersion Raceband
#endif
};

// All Channels of the above List ordered by Mhz
const uint8_t channelList[] PROGMEM = {
#ifdef USE_LBAND
  40, 41, 42, 43, 44, 45, 46, 47, 19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
#else
  19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
#endif
};

char channel = 0;
uint8_t channelIndex = 0;
uint8_t rssi = 0;
uint8_t rssi_scaled = 0;
uint8_t active_receiver = useReceiverA;
#ifdef USE_DIVERSITY
    uint8_t diversity_mode = useReceiverAuto;
    char diversity_check_count = 0; // used to decide when to change antennas.
#endif
uint8_t rssi_seek_threshold = RSSI_SEEK_TRESHOLD;
uint8_t hight = 0;
uint8_t state = START_STATE;
uint8_t state_last_used=START_STATE;
uint8_t last_state= START_STATE+1; // force screen draw
uint8_t writePos = 0;
uint8_t switch_count = 0;
uint8_t man_channel = 0;
uint8_t last_channel_index = 0;
uint8_t force_seek=0;
uint8_t seek_direction=1;
unsigned long time_of_tune = 0;        // will store last time when tuner was changed
unsigned long time_screen_saver = 0;
unsigned long time_next_payload = 0;
uint8_t last_active_channel=0;
uint8_t seek_found=0;
uint8_t last_dip_channel=255;
uint8_t last_dip_band=255;
uint8_t scan_start=0;
uint8_t first_tune=1;
boolean force_menu_redraw=0;
uint16_t rssi_best=0; // used for band scaner
uint16_t rssi_min_a=RSSI_MIN_VAL;
uint16_t rssi_max_a=RSSI_MAX_VAL;
uint16_t rssi_setup_min_a=RSSI_MIN_VAL;
uint16_t rssi_setup_max_a=RSSI_MAX_VAL;
#ifdef USE_DIVERSITY
    uint16_t rssi_min_b=RSSI_MIN_VAL;
    uint16_t rssi_max_b=RSSI_MAX_VAL;
    uint16_t rssi_setup_min_b=RSSI_MIN_VAL;
    uint16_t rssi_setup_max_b=RSSI_MAX_VAL;
#endif
uint8_t rssi_setup_run=0;

#ifdef USE_VOLTAGE_MONITORING
int vbat_scale = VBAT_SCALE;
uint8_t warning_voltage = WARNING_VOLTAGE;
uint8_t critical_voltage = CRITICAL_VOLTAGE;
boolean critical_alarm = false;
boolean warning_alarm = false;
uint8_t beep_times = 0;
boolean beeping = false;
unsigned long time_last_vbat_alarm = 0;
unsigned long last_beep = 0;

#define VBAT_SMOOTH 8
#define VBAT_PRESCALER 16
uint8_t voltage_reading_index = 0;
uint16_t voltages[VBAT_SMOOTH];
uint16_t voltages_sum;
uint16_t voltage;
#endif

char call_sign[10];
bool settings_beeps = true;
bool settings_orderby_channel = true;

// SETUP ----------------------------------------------------------------------------
void setup()
{
    // IO INIT
    // initialize digital pin 13 LED as an output.
    pinMode(led, OUTPUT); // status pin for TV mode errors
    digitalWrite(led, HIGH);
    // buzzer
    pinMode(buzzer, OUTPUT); // Feedback buzzer (active buzzer, not passive piezo)
    digitalWrite(buzzer, HIGH);
    // minimum control pins
    pinMode(buttonUp, INPUT);
    digitalWrite(buttonUp, INPUT_PULLUP);
    pinMode(buttonMode, INPUT);
    digitalWrite(buttonMode, INPUT_PULLUP);
    // optional control
    pinMode(buttonDown, INPUT);
    digitalWrite(buttonDown, INPUT_PULLUP);
    pinMode(buttonSave, INPUT);
    digitalWrite(buttonSave, INPUT_PULLUP);
    //Receiver Setup
    pinMode(receiverA_led,OUTPUT);
#ifdef USE_DIVERSITY
    pinMode(receiverB_led,OUTPUT);
#endif
    setReceiver(useReceiverA);
    // SPI pins for RX control
    pinMode (slaveSelectPin, OUTPUT);
    pinMode (spiDataPin, OUTPUT);
	pinMode (spiClockPin, OUTPUT);

    // use values only of EEprom is not 255 = unsaved
    uint8_t eeprom_check = EEPROM.read(EEPROM_ADR_STATE);
    if(eeprom_check == 255) // unused
    {
        // save 8 bit
        EEPROM.write(EEPROM_ADR_STATE,START_STATE);
        EEPROM.write(EEPROM_ADR_TUNE,CHANNEL_MIN_INDEX);
        EEPROM.write(EEPROM_ADR_BEEP,settings_beeps);
        EEPROM.write(EEPROM_ADR_ORDERBY,settings_orderby_channel);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,highByte(RSSI_MAX_VAL));

        // save default call sign
        strcpy(call_sign, CALL_SIGN); // load callsign
        for(uint8_t i = 0;i<sizeof(call_sign);i++) {
            EEPROM.write(EEPROM_ADR_CALLSIGN+i,call_sign[i]);
        }



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

#ifdef USE_VOLTAGE_MONITORING
        EEPROM.write(EEPROM_ADR_VBAT_SCALE, vbat_scale);
        EEPROM.write(EEPROM_ADR_VBAT_WARNING, warning_voltage);
        EEPROM.write(EEPROM_ADR_VBAT_CRITICAL, critical_voltage);
#endif

    }

    // read last setting from eeprom
    state=EEPROM.read(EEPROM_ADR_STATE);
    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);
    // set the channel as soon as we can
    // faster boot up times :)
    setChannelModule(channelIndex);
    last_channel_index=channelIndex;

    settings_beeps=EEPROM.read(EEPROM_ADR_BEEP);
    settings_orderby_channel=EEPROM.read(EEPROM_ADR_ORDERBY);

    // load saved call sign
    for(uint8_t i = 0;i<sizeof(call_sign);i++) {
        call_sign[i] = EEPROM.read(EEPROM_ADR_CALLSIGN+i);
    }

    rssi_min_a=((EEPROM.read(EEPROM_ADR_RSSI_MIN_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_A_L)));
    rssi_max_a=((EEPROM.read(EEPROM_ADR_RSSI_MAX_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_A_L)));
#ifdef USE_DIVERSITY
    diversity_mode = EEPROM.read(EEPROM_ADR_DIVERSITY);
    rssi_min_b=((EEPROM.read(EEPROM_ADR_RSSI_MIN_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_B_L)));
    rssi_max_b=((EEPROM.read(EEPROM_ADR_RSSI_MAX_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_B_L)));
#endif
    force_menu_redraw=1;

    // Init Display
    if (drawScreen.begin(call_sign) > 0) {
        // on Error flicker LED
        while (true) { // stay in ERROR for ever
            digitalWrite(led, !digitalRead(led));
            delay(100);
        }
    }

#ifdef USE_IR_EMITTER
    // Used to Transmit IR Payloads
    Serial.begin(9600);
#endif

#ifdef USE_DIVERSITY
    // make sure we use receiver Auto when diveristy is unplugged.
    if(!isDiversity()) {
        diversity_mode = useReceiverAuto;
    }
#endif
#ifdef USE_VOLTAGE_MONITORING
        vbat_scale = EEPROM.read(EEPROM_ADR_VBAT_SCALE);
        warning_voltage = EEPROM.read(EEPROM_ADR_VBAT_WARNING);
        critical_voltage = EEPROM.read(EEPROM_ADR_VBAT_CRITICAL);
#endif
    // Setup Done - Turn Status LED off.
    digitalWrite(led, LOW);

}


// LOOP ----------------------------------------------------------------------------
void loop()
{
    /*******************/
    /*   Mode Select   */
    /*******************/
    uint8_t in_menu;
    uint8_t in_menu_time_out;

    if (digitalRead(buttonMode) == LOW) // key pressed ?
    {
#ifdef USE_VOLTAGE_MONITORING
        clear_alarm();
#endif
        time_screen_saver=0;
        beep(50); // beep & debounce
        delay(KEY_DEBOUNCE/2); // debounce
        beep(50); // beep & debounce
        delay(KEY_DEBOUNCE/2); // debounce

        uint8_t press_time=0;
        // on entry wait for release
        while(digitalRead(buttonMode) == LOW && press_time < 10)
        {
            delay(100);
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
        in_menu=1;
        in_menu_time_out=50; // 20x 100ms = 5 seconds
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
                    if(isDiversity()) {
                        state=STATE_DIVERSITY;
                    }
                    else {
                        menu_id++;
                        state=STATE_SETUP_MENU;
                    }
                break;
            #else
                case 3: // Skip
                    menu_id++;
            #endif
                case 4: // Setup Menu
                    state=STATE_SETUP_MENU;
                break;
            } // end switch

            // draw mode select screen
            drawScreen.mainMenu(menu_id);

            while(digitalRead(buttonMode) == LOW || digitalRead(buttonUp) == LOW || digitalRead(buttonDown) == LOW)
            {
                // wait for MODE release
                in_menu_time_out=50;
            }
            while(--in_menu_time_out && ((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH))) // wait for next key press or time out
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
                if(digitalRead(buttonUp) == LOW) {
                    menu_id--;
#ifdef USE_DIVERSITY
                    if(!isDiversity() && menu_id == 3) { // make sure we back up two menu slots.
                        menu_id--;
                    }
#else
                    if(menu_id == 3) { // as we dont use diveristy make sure we back up two menu slots.
                        menu_id--;
                    }
#endif
                }
                else if(digitalRead(buttonDown) == LOW) {
                    menu_id++;
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

        // simple menu
        switch (state)
        {
            case STATE_SCAN: // Band Scanner
                state_last_used=state;
            case STATE_RSSI_SETUP: // RSSI setup
                // draw selected
                if(state==STATE_RSSI_SETUP)
                {
                    // prepare new setup
                    rssi_min_a=50;
                    rssi_max_a=300; // set to max range
                    rssi_setup_min_a=RSSI_MAX_VAL;
                    rssi_setup_max_a=RSSI_MIN_VAL;
#ifdef USE_DIVERSITY
                    rssi_min_b=50;
                    rssi_max_b=300; // set to max range
                    rssi_setup_min_b=RSSI_MAX_VAL;
                    rssi_setup_max_b=RSSI_MIN_VAL;
#endif
                    rssi_setup_run=RSSI_SETUP_RUN;
                }

                // trigger new scan from begin
                channel=CHANNEL_MIN;
                channelIndex = pgm_read_byte_near(channelList + channel);
                rssi_best=0;
                scan_start=1;

                drawScreen.bandScanMode(state);
            break;
            case STATE_SEEK: // seek mode
                rssi_seek_threshold = RSSI_SEEK_TRESHOLD;
                rssi_best=0;
                force_seek=1;
            case STATE_MANUAL: // manual mode
                if (state == STATE_MANUAL)
                {
                    time_screen_saver=millis();
                }
                else if(state == STATE_SEEK)
                {
                    time_screen_saver=0; // dont show screen saver until we found a channel.
                }
                drawScreen.seekMode(state);

                // return user to their saved channel after bandscan
                if(state_last_used == STATE_SCAN || last_state == STATE_RSSI_SETUP) {
                    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);
                }
                state_last_used=state;
            break;
#ifdef USE_DIVERSITY
            case STATE_DIVERSITY:
                // diversity menu is below this is just a place holder.
            break;
#endif
#ifdef USE_VOLTAGE_MONITORING
            case STATE_VOLTAGE:
                // voltage menu below
            break;
#endif
            case STATE_SETUP_MENU:

            break;
            case STATE_SAVE:
                EEPROM.write(EEPROM_ADR_TUNE,channelIndex);
                EEPROM.write(EEPROM_ADR_STATE,state_last_used);
                EEPROM.write(EEPROM_ADR_BEEP,settings_beeps);
                EEPROM.write(EEPROM_ADR_ORDERBY,settings_orderby_channel);
                // save call sign
                for(uint8_t i = 0;i<sizeof(call_sign);i++) {
                    EEPROM.write(EEPROM_ADR_CALLSIGN+i,call_sign[i]);
                }
#ifdef USE_DIVERSITY
                EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
#endif

#ifdef USE_VOLTAGE_MONITORING
                EEPROM.write(EEPROM_ADR_VBAT_SCALE, vbat_scale);
                EEPROM.write(EEPROM_ADR_VBAT_WARNING, warning_voltage);
                EEPROM.write(EEPROM_ADR_VBAT_CRITICAL, critical_voltage);
#endif
                drawScreen.save(state_last_used, channelIndex, pgm_read_word_near(channelFreqTable + channelIndex), call_sign);
                for (uint8_t loop=0;loop<5;loop++)
                {
                    beep(100); // beep
                    delay(100);
                }
                delay(3000);
                state=state_last_used; // return to saved function
                force_menu_redraw=1; // we change the state twice, must force redraw of menu

            // selection by inverted box
            break;
        } // end switch

        last_state=state;
    }
    /*************************************/
    /*   Processing depending of state   */
    /*************************************/
#ifndef TVOUT_SCREENS
    if(state == STATE_SCREEN_SAVER) {
#ifdef USE_DIVERSITY
        drawScreen.screenSaver(diversity_mode, pgm_read_byte_near(channelNames + channelIndex), pgm_read_word_near(channelFreqTable + channelIndex), call_sign);
#else
        drawScreen.screenSaver(pgm_read_byte_near(channelNames + channelIndex), pgm_read_word_near(channelFreqTable + channelIndex), call_sign);
#endif
#ifdef USE_VOLTAGE_MONITORING
            read_voltage();
            voltage_alarm();
            drawScreen.updateVoltageScreenSaver(voltage, warning_alarm || critical_alarm);
#endif
        do{
            rssi = readRSSI();

#ifdef USE_DIVERSITY
            drawScreen.updateScreenSaver(active_receiver, rssi, readRSSI(useReceiverA), readRSSI(useReceiverB));
#else
            drawScreen.updateScreenSaver(rssi);
#endif

#ifdef USE_VOLTAGE_MONITORING
            read_voltage();
            voltage_alarm();

            drawScreen.updateVoltageScreenSaver(voltage, warning_alarm || critical_alarm);
#endif
        }
        while((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)); // wait for next button press
        state=state_last_used;
        time_screen_saver=0;
        return;
    }
#endif
#ifdef USE_VOLTAGE_MONITORING
    if(state == STATE_VOLTAGE) {
        // simple menu
        char menu_id=0;
        uint8_t in_voltage_menu=1;
        int editing = -1;
        do{
            drawScreen.voltage(menu_id, vbat_scale, warning_voltage, critical_voltage);
            do {
                drawScreen.updateVoltage(voltage);
                read_voltage();
                voltage_alarm();
                //delay(100); // timeout delay
            }
            while((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)); // wait for next key press

            if(digitalRead(buttonMode) == LOW){
                if(editing > -1){
                    // user is done editing
                    editing = -1;
                }
                else if(menu_id < 3)
                {
                    editing = menu_id;
                }
                else if(menu_id == 3)
                {
                    in_menu = 0; // save & exit menu
                    in_voltage_menu = 0; // save & exit menu
                    state=STATE_SAVE;
                    editing = -1;
                }
            } else if(digitalRead(buttonDown) == LOW) {
                switch (editing) {
                    case 0:
                        warning_voltage--;
                        break;
                    case 1:
                        critical_voltage--;
                        break;
                    case 2:
                        vbat_scale--;
                        break;
                    default:
                        menu_id++;
                        break;
                }
            }
            else if(digitalRead(buttonUp) == LOW) {
                switch (editing) {
                    case 0:
                        warning_voltage++;
                        break;
                    case 1:
                        critical_voltage++;
                        break;
                    case 2:
                        vbat_scale++;
                        break;
                    default:
                        menu_id--;
                        break;
                }
            }

            if(menu_id > 3) {
                menu_id = 0;
            }
            if(menu_id < 0) {
                menu_id = 3;
            }
            beep(50); // beep & debounce
            //delay(KEY_DEBOUNCE); // debounce
            do{
                delay(150);// wait for button release
            }
            while(editing==-1 && (digitalRead(buttonMode) == LOW || digitalRead(buttonUp) == LOW || digitalRead(buttonDown) == LOW));
        }
        while(in_voltage_menu);
    }
#endif

#ifdef USE_DIVERSITY
    if(state == STATE_DIVERSITY) {
        // simple menu
        char menu_id=diversity_mode;
        uint8_t in_menu=1;
        do{
            diversity_mode = menu_id;
            drawScreen.diversity(diversity_mode);
            do
            {
                //delay(10); // timeout delay
                readRSSI();
                drawScreen.updateDiversity(active_receiver, readRSSI(useReceiverA), readRSSI(useReceiverB));
            }
            while((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)); // wait for next mode or time out

            if(digitalRead(buttonMode) == LOW)        // channel UP
            {
                in_menu = 0; // exit menu
            }
            else if(digitalRead(buttonUp) == LOW) {
                menu_id--;
            }
            else if(digitalRead(buttonDown) == LOW) {
                menu_id++;
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
        // read rssi
        wait_rssi_ready();
        rssi = readRSSI();
        rssi_best = (rssi > rssi_best) ? rssi : rssi_best;

        channel=channel_from_index(channelIndex); // get 0...48 index depending of current channel
        if(state == STATE_MANUAL) // MANUAL MODE
        {
#ifdef USE_IR_EMITTER
            if(time_next_payload+1000 < millis() && rssi <= 50) { // send channel info every second until rssi is locked.
                sendIRPayload();
                time_next_payload = millis();
            }
#endif
            // handling of keys
            if( digitalRead(buttonUp) == LOW)        // channel UP
            {
                time_screen_saver=millis();
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
                channelIndex++;
                channel++;
                channel > CHANNEL_MAX ? channel = CHANNEL_MIN : false;
                if (channelIndex > CHANNEL_MAX_INDEX)
                {
                    channelIndex = CHANNEL_MIN_INDEX;
                }
            }
            if( digitalRead(buttonDown) == LOW) // channel DOWN
            {
                time_screen_saver=millis();
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
                channelIndex--;
                channel--;
                channel < CHANNEL_MIN ? channel = CHANNEL_MAX : false;
                if (channelIndex > CHANNEL_MAX_INDEX) // negative overflow
                {
                    channelIndex = CHANNEL_MAX_INDEX;
                }
            }

            if(!settings_orderby_channel) { // order by frequency
                channelIndex = pgm_read_byte_near(channelList + channel);
            }

        }

        // handling for seek mode after screen and RSSI has been fully processed
        if(state == STATE_SEEK) //
        { // SEEK MODE

            // recalculate rssi_seek_threshold
            ((int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0)) > rssi_seek_threshold) ? (rssi_seek_threshold = (int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0))) : false;

            if(!seek_found) // search if not found
            {
                if ((!force_seek) && (rssi > rssi_seek_threshold)) // check for found channel
                {
                    seek_found=1;
                    time_screen_saver=millis();
                    // beep twice as notice of lock
                    beep(100);
                    delay(100);
                    beep(100);
                }
                else
                { // seeking itself
                    force_seek=0;
                    // next channel
                    channel+=seek_direction;
                    if (channel > CHANNEL_MAX)
                    {
                        // calculate next pass new seek threshold
                        rssi_seek_threshold = (int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0));
                        channel=CHANNEL_MIN;
                        rssi_best = 0;
                    }
                    else if(channel < CHANNEL_MIN)
                    {
                        // calculate next pass new seek threshold
                        rssi_seek_threshold = (int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0));
                        channel=CHANNEL_MAX;
                        rssi_best = 0;
                    }
                    rssi_seek_threshold = rssi_seek_threshold < 5 ? 5 : rssi_seek_threshold; // make sure we are not stopping on everyting
                    channelIndex = pgm_read_byte_near(channelList + channel);
                }
            }
            else
            { // seek was successful

            }
            if (digitalRead(buttonUp) == LOW || digitalRead(buttonDown) == LOW) // restart seek if key pressed
            {
                if(digitalRead(buttonUp) == LOW) {
                    seek_direction = 1;
                }
                else {
                    seek_direction = -1;
                }
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
                force_seek=1;
                seek_found=0;
                time_screen_saver=0;
            }
        }
#ifndef TVOUT_SCREENS
        // change to screensaver after lock and 5 seconds has passed.
        if(time_screen_saver+5000 < millis() && time_screen_saver != 0 && rssi > 50 ||
            (time_screen_saver != 0 && time_screen_saver + (SCREENSAVER_TIMEOUT*1000) < millis())) {
            state = STATE_SCREEN_SAVER;
        }
#endif
        drawScreen.updateSeekMode(state, channelIndex, channel, rssi, pgm_read_word_near(channelFreqTable + channelIndex), rssi_seek_threshold, seek_found, voltage);
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
        }

        // print bar for spectrum
        wait_rssi_ready();
        // value must be ready
        rssi = readRSSI();

        if(state == STATE_SCAN)
        {
            if (rssi > RSSI_SEEK_TRESHOLD)
            {
                if(rssi_best < rssi) {
                    rssi_best = rssi;
                }
            }
        }

        uint8_t bestChannelName = pgm_read_byte_near(channelNames + channelIndex);
        uint16_t bestChannelFrequency = pgm_read_word_near(channelFreqTable + channelIndex);

        drawScreen.updateBandScanMode((state == STATE_RSSI_SETUP), channel, rssi, bestChannelName, bestChannelFrequency, rssi_setup_min_a, rssi_setup_max_a);

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
                    if(rssi_max_a < 125) { // user probably did not turn on the VTX during calibration
                        rssi_max_a = RSSI_MAX_VAL;
                    }
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,(rssi_min_a & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,(rssi_min_a >> 8));
                    // save 16 bit
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,(rssi_max_a & 0xff));
                    EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,(rssi_max_a >> 8));

#ifdef USE_DIVERSITY

                    if(isDiversity()) { // only calibrate RSSI B when diversity is detected.
                        rssi_min_b=rssi_setup_min_b;
                        rssi_max_b=rssi_setup_max_b;
                        if(rssi_max_b < 125) { // user probably did not turn on the VTX during calibration
                            rssi_max_b = RSSI_MAX_VAL;
                        }
                        // save 16 bit
                        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,(rssi_min_b & 0xff));
                        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,(rssi_min_b >> 8));
                        // save 16 bit
                        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,(rssi_max_b & 0xff));
                        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,(rssi_max_b >> 8));
                    }
#endif
                    state=EEPROM.read(EEPROM_ADR_STATE);
                    beep(1000);
                }
            }
        }
        // new scan possible by press scan
        if (digitalRead(buttonUp) == LOW) // force new full new scan
        {
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            last_state=255; // force redraw by fake state change ;-)
            channel=CHANNEL_MIN;
            scan_start=1;
            rssi_best=0;
        }
        // update index after channel change
        channelIndex = pgm_read_byte_near(channelList + channel);
    }


    if(state == STATE_SETUP_MENU)
    {
        // simple menu
        char menu_id=0;
        in_menu=1;
        drawScreen.setupMenu();
        int editing = -1;
        do{
            in_menu_time_out=80;
            drawScreen.updateSetupMenu(menu_id, settings_beeps, settings_orderby_channel, call_sign, editing);
            while(--in_menu_time_out && ((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH))) // wait for next key press or time out
            {
                delay(100); // timeout delay
            }

            if(in_menu_time_out <= 0 ) {
                state = state_last_used;
                break; // Timed out, Don't save...
            }

            if(digitalRead(buttonMode) == LOW)        // modeButton
            {
                // do something about the users selection
                switch(menu_id) {
                    case 0: // Channel Order Channel/Frequency
                        settings_orderby_channel = !settings_orderby_channel;
                        break;
                    case 1:// Beeps enable/disable
                        settings_beeps = !settings_beeps;
                        break;

                    case 2:// Edit Call Sign
                        editing++;
                        if(editing>9) {
                            editing=-1;
                        }
                        break;
                    case 3:// Calibrate RSSI
                        in_menu = 0;
                        for (uint8_t loop=0;loop<10;loop++)
                        {
                            #define RSSI_SETUP_BEEP 25
                            beep(RSSI_SETUP_BEEP); // beep & debounce
                            delay(RSSI_SETUP_BEEP); // debounce
                        }
                        state=STATE_RSSI_SETUP;
                        break;
#ifdef USE_VOLTAGE_MONITORING
                    case 4:// Change Voltage Settings
                        in_menu = 0;
                        state=STATE_VOLTAGE;
                        break;
                    case 5:
#else
                    case 4:
#endif
                        in_menu = 0; // save & exit menu
                        state=STATE_SAVE;
                        break;

                }
            }
            else if(digitalRead(buttonUp) == LOW) {
                if(editing == -1) {
                    menu_id--;
#ifdef TVOUT_SCREENS
                    if(menu_id == 2) {
                        menu_id--;
                    }
#endif
                }
                else { // change current letter in place
                    call_sign[editing]++;
                    call_sign[editing] > '}' ? call_sign[editing] = ' ' : false; // loop to oter end
                }

            }
            else if(digitalRead(buttonDown) == LOW) {
                if(editing == -1) {
                    menu_id++;

#ifdef TVOUT_SCREENS
                    if(menu_id == 2) {
                        menu_id++;
                    }
#endif
                }
                else { // change current letter in place
                    call_sign[editing]--;
                    call_sign[editing] < ' ' ? call_sign[editing] = '}' : false; // loop to oter end
                }
            }

            if(menu_id > SETUP_MENU_MAX_ITEMS) {
                menu_id = 0;
            }
            if(menu_id < 0) {
                menu_id = SETUP_MENU_MAX_ITEMS;
            }

            beep(50); // beep & debounce
            do{
                delay(150);// wait for button release
            }
            while(editing==-1 && (digitalRead(buttonMode) == LOW || digitalRead(buttonUp) == LOW || digitalRead(buttonDown) == LOW));
        }
        while(in_menu);
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
#ifdef USE_VOLTAGE_MONITORING
    read_voltage();
    voltage_alarm();
#endif
}

/*###########################################################################*/
/*******************/
/*   SUB ROUTINES  */
/*******************/

void beep(uint16_t time)
{
    digitalWrite(led, HIGH);
    if(settings_beeps){
        digitalWrite(buzzer, LOW); // activate beep
    }
    delay(time/2);
    digitalWrite(led, LOW);
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
uint16_t readRSSI(char receiver)
{
#endif
    int rssi = 0;
    int rssiA = 0;

#ifdef USE_DIVERSITY
    int rssiB = 0;
#endif
    for (uint8_t i = 0; i < RSSI_READS; i++)
    {
        rssiA += analogRead(rssiPinA);//random(RSSI_MAX_VAL-200, RSSI_MAX_VAL);//

#ifdef USE_DIVERSITY
        rssiB += analogRead(rssiPinB);//random(RSSI_MAX_VAL-200, RSSI_MAX_VAL);//
#endif
    }
    rssiA = rssiA/RSSI_READS; // average of RSSI_READS readings

#ifdef USE_DIVERSITY
    rssiB = rssiB/RSSI_READS; // average of RSSI_READS readings
#endif
    // special case for RSSI setup
    if(state==STATE_RSSI_SETUP)
    { // RSSI setup
        if(rssiA < rssi_setup_min_a)
        {
            rssi_setup_min_a=rssiA;
        }
        if(rssiA > rssi_setup_max_a)
        {
            rssi_setup_max_a=rssiA;
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

    rssiA = map(rssiA, rssi_min_a, rssi_max_a , 1, 100);   // scale from 1..100%
#ifdef USE_DIVERSITY
    rssiB = map(rssiB, rssi_min_b, rssi_max_b , 1, 100);   // scale from 1..100%
    if(receiver == -1) // no receiver was chosen using diversity
    {
        switch(diversity_mode)
        {
            case useReceiverAuto:
                // select receiver
                if((int)abs((float)(((float)rssiA - (float)rssiB) / (float)rssiB) * 100.0) >= DIVERSITY_CUTOVER)
                {
                    if(rssiA > rssiB && diversity_check_count > 0)
                    {
                        diversity_check_count--;
                    }
                    if(rssiA < rssiB && diversity_check_count < DIVERSITY_MAX_CHECKS)
                    {
                        diversity_check_count++;
                    }
                    // have we reached the maximum number of checks to switch receivers?
                    if(diversity_check_count == 0 || diversity_check_count >= DIVERSITY_MAX_CHECKS) {
                        receiver=(diversity_check_count == 0) ? useReceiverA : useReceiverB;
                    }
                    else {
                        receiver=active_receiver;
                    }
                }
                else {
                    receiver=active_receiver;
                }
                break;
            case useReceiverB:
                receiver=useReceiverB;
                break;
            case useReceiverA:
            default:
                receiver=useReceiverA;
        }
        // set the antenna LED and switch the video
        setReceiver(receiver);
    }
#endif

#ifdef USE_DIVERSITY
    if(receiver == useReceiverA || state==STATE_RSSI_SETUP)
    {
#endif
        rssi = rssiA;
#ifdef USE_DIVERSITY
    }
    else {
        rssi = rssiB;
    }
#endif
    return constrain(rssi,1,100); // clip values to only be within this range.
}

void setReceiver(uint8_t receiver) {
#ifdef USE_DIVERSITY
    if(receiver == useReceiverA)
    {
        digitalWrite(receiverB_led, LOW);
        digitalWrite(receiverA_led, HIGH);
    }
    else
    {
        digitalWrite(receiverA_led, LOW);
        digitalWrite(receiverB_led, HIGH);
    }
#else
    digitalWrite(receiverA_led, HIGH);
#endif

    active_receiver = receiver;
}


#ifdef USE_IR_EMITTER
void sendIRPayload() {
    // beep twice before transmitting.
    beep(100);
    delay(100);
    beep(100);
    uint8_t check_sum = 2;
    Serial.write(2); // start of payload STX
    check_sum += channelIndex;
    Serial.write(channelIndex); // send channel
    for(uint8_t i=0; i < 10;i++) {
        if(call_sign[i] == '\0') {
            break;
        }
        check_sum += (char)call_sign[i];
        Serial.write(call_sign[i]); // send char of call_sign
    }
    Serial.write(3);  // end of payload ETX
    Serial.write(check_sum); // send ceck_sum for payload validation
}
#endif

void setChannelModule(uint8_t channel)
{
  uint8_t i;
  uint16_t channelData;

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


#ifdef USE_VOLTAGE_MONITORING
void read_voltage()
{
    uint16_t v = analogRead(VBAT_PIN);
    voltages_sum += v;
    voltages_sum -= voltages[voltage_reading_index];
    voltages[voltage_reading_index++] = v;
    voltage_reading_index %= VBAT_SMOOTH;
#if VBAT_SMOOTH == VBAT_PRESCALER
    voltage = voltages_sum / vbat_scale + VBAT_OFFSET; // result is Vbatt in 0.1V steps
#elif VBAT_SMOOTH < VBAT_PRESCALER
    voltage = (voltages_sum * (VBAT_PRESCALER/VBAT_SMOOTH)) / vbat_scale + VBAT_OFFSET; // result is Vbatt in 0.1V steps
#else
    voltage = ((voltages_sum /VBAT_SMOOTH) * VBAT_PRESCALER) / vbat_scale + VBAT_OFFSET; // result is Vbatt in 0.1V steps
#endif
    if(voltage <= critical_voltage) {
        critical_alarm = true;
        warning_alarm = false;
    } else if(voltage <= warning_voltage) {
        warning_alarm = true;
        critical_alarm = false;
    } else {
        critical_alarm = false;
        warning_alarm = false;
    }
}
void voltage_alarm(){
    if(millis() > time_last_vbat_alarm + ALARM_EVERY_MSEC){
        if(critical_alarm){
            //continue playint the critical alarm
            if(millis() - CRITICAL_BEEP_EVERY_MSEC > last_beep){
                //flip the beeper output
                set_buzzer(beeping);
                beeping = !beeping;
                last_beep = millis();
                beep_times++;
            }
            if(beep_times > (CRITICAL_BEEPS*2)) {
                //stop the beeping if we already beeped enough times
                clear_alarm();
                time_last_vbat_alarm = millis();
            }
        } else if(warning_alarm) {
            //continue playint the warning alarm
            if(millis() - WARNING_BEEP_EVERY_MSEC > last_beep){
                //flip the beeper output
                set_buzzer(beeping);
                beeping = !beeping;
                last_beep = millis();
                beep_times++;
            }
            if(beep_times > (WARNING_BEEPS*2)) {
                //stop the beeping if we already beeped enough times
                clear_alarm();
                time_last_vbat_alarm = millis();
            }
        }
    }
}
void clear_alarm(){
    //stop alarm sound when we are at menu etc
    // it might be problematic when were in the middle of a alarm sound
    set_buzzer(false);
    beep_times = 0;
}
void set_buzzer(boolean value){
    digitalWrite(led, value);
    digitalWrite(buzzer, !value);
}
#endif
