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

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"

//#include "screens.h"
#include "channels.h"
#include "receiver.h"
#include "buttons.h"

#include "state.h"
#include "state_scan.h"
#include "state_manual.h"
#include "state_screensaver.h"
#include "state_auto.h"

#include "ui.h"


#ifdef OLD_LOOP
//screens drawScreen;
char channel = 0;
uint8_t channelIndex = 0;
uint8_t rssi = 0;
#ifdef USE_DIVERSITY
    uint8_t diversity_mode = RECEIVER_AUTO;
    char diversity_check_count = 0; // used to decide when to change antennas.
#endif
uint8_t rssi_seek_threshold = RSSI_SEEK_TRESHOLD;
uint8_t state = START_STATE;
uint8_t state_last_used=START_STATE;
uint8_t last_state= START_STATE+1; // force screen draw
uint8_t switch_count = 0;
uint8_t last_channel_index = 0;
uint8_t force_seek=0;
uint8_t seek_direction=1;
unsigned long time_of_tune = 0;        // will store last time when tuner was changed
unsigned long time_screen_saver = 0;
unsigned long time_next_payload = 0;
uint8_t seek_found=0;
uint8_t scan_start=0;
uint8_t first_tune=1;
bool force_menu_redraw=0;
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
bool critical_alarm = false;
bool warning_alarm = false;
uint8_t beep_times = 0;
bool beeping = false;
unsigned long time_last_vbat_alarm = 0;
unsigned long last_beep = 0;

uint8_t voltage_reading_index = 0;
uint16_t voltages[VBAT_SMOOTH];
uint16_t voltages_sum;
uint16_t voltage;
#endif

char call_sign[10];
bool settings_beeps = true;
bool settings_orderby_channel = true;

void beep(uint8_t count = 1, uint16_t milliseconds = 100);
uint8_t readRSSI(char receiver = -1);
#endif


#ifndef OLD_LOOP


//screens drawScreen;


#endif


// === Setup ===================================================================

void setup()
{
    setupPins();

    // Enable buzzer and LED for duration of setup process.
    digitalWrite(PIN_LED, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);

    Receiver::setActiveReceiver(RECEIVER_A);
    setupSettings();

    // Init Display
    /*if (drawScreen.begin(nullptr) > 0) {
        haltWithError();
    }*/

    Ui::setup();

    #ifdef USE_IR_EMITTER
        // Used to Transmit IR Payloads
        Serial.begin(9600);
    #endif

    #ifdef USE_SERIAL_OUT
        Serial.begin(250000);
    #endif

    // Setup complete.
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_BUZZER, LOW);

    // Switch to initial state.
    StateMachine::switchState(StateMachine::State::MENU);
}

void setupPins() {
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    pinMode(PIN_BUTTON_UP, INPUT);
    pinMode(PIN_BUTTON_MODE, INPUT);
    digitalWrite(PIN_BUTTON_UP, INPUT_PULLUP);
    digitalWrite(PIN_BUTTON_MODE, INPUT_PULLUP);

    pinMode(PIN_BUTTON_DOWN, INPUT);
    pinMode(PIN_BUTTON_SAVE, INPUT);
    digitalWrite(PIN_BUTTON_DOWN, INPUT_PULLUP);
    digitalWrite(PIN_BUTTON_SAVE, INPUT_PULLUP);

    pinMode(PIN_LED_A,OUTPUT);
    #ifdef USE_DIVERSITY
        pinMode(PIN_LED_B,OUTPUT);
    #endif

    pinMode(PIN_SPI_SLAVE_SELECT, OUTPUT);
    pinMode(PIN_SPI_DATA, OUTPUT);
	pinMode(PIN_SPI_CLOCK, OUTPUT);

    // Enabe pull-up resistors on RSSI pins to help keep the receivers a little
    // cooler.
    digitalWrite(PIN_RSSI_A, HIGH);
    #ifdef USE_DIVERSITY
        digitalWrite(PIN_RSSI_B, HIGH);
    #endif
}

void setupSettings() {
    EepromSettings.load();
    Receiver::setChannel(EepromSettings.channel);
}

// === Main Loop ===============================================================

#ifndef OLD_LOOP


void loop() {
    Receiver::update();
    ButtonState::update();
    StateMachine::tick();
    Ui::update();

    if (
        StateMachine::currentState != StateMachine::State::SCREENSAVER
        && (millis() - ButtonState::lastPressTime) >
            (SCREENSAVER_TIMEOUT * 1000)
    ) {
        StateMachine::switchState(StateMachine::State::SCREENSAVER);
    }
}


#endif

#ifdef OLD_LOOP
void loop()
{
    /*******************/
    /*   Mode Select   */
    /*******************/
    uint8_t in_menu;
    uint8_t in_menu_time_out;

    Receiver::updateRssi();
    ButtonState::update();
    rssi = Receiver::rssiA;

    if (ButtonState::get(Button::MODE)) {
        #ifdef USE_VOLTAGE_MONITORING
            clearAlarm();
        #endif

        time_screen_saver=0;
        //beep(2); // beep & debounce

        // Weird divide by 10 here because press_time is in deciseconds.
        uint8_t press_time = ButtonState::waitForRelease(Button::MODE) / 100;
        #define MAX_MENU 4
        #define MENU_Y_SIZE 15

        char menu_id=state_last_used-1;
        // Show Mode Screen
        if(state==STATE_SEEK_FOUND) {
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

            while(digitalRead(PIN_BUTTON_MODE) == LOW || digitalRead(PIN_BUTTON_UP) == LOW || digitalRead(PIN_BUTTON_DOWN) == LOW)
            {
                // wait for MODE release
                in_menu_time_out=50;
            }
            while(--in_menu_time_out && ((digitalRead(PIN_BUTTON_MODE) == HIGH) && (digitalRead(PIN_BUTTON_UP) == HIGH) && (digitalRead(PIN_BUTTON_DOWN) == HIGH))) // wait for next key press or time out
            {
                delay(100); // timeout delay
            }
            if(in_menu_time_out==0 || digitalRead(PIN_BUTTON_MODE) == LOW)
            {
                if(digitalRead(PIN_BUTTON_MODE) != LOW) {
                    state=state_last_used; // exit to last state on timeout.
                }
                in_menu=0; // EXIT
                beep(2);
            }
            else // no timeout, must be keypressed
            {
                /*********************/
                /*   Menu handler   */
                /*********************/
                if(digitalRead(PIN_BUTTON_UP) == LOW) {
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
                else if(digitalRead(PIN_BUTTON_DOWN) == LOW) {
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
                beep(); // beep & debounce
                delay(BUTTON_DEBOUNCE_DELAY); // debounce
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
    if (ButtonState::get(Button::SAVE)) {
        state = STATE_SAVE;
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
                channelIndex = pgm_read_byte_near(channelFreqOrderedIndex + channel);
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
                    channelIndex = EepromSettings.channel;
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
                EepromSettings.channel = channelIndex;
                EepromSettings.defaultState = state_last_used;
                EepromSettings.beepEnabled = settings_beeps;
                EepromSettings.orderByChannel = settings_orderby_channel;
                memcpy(EepromSettings.callSign, call_sign, sizeof(call_sign));

                #ifdef USE_DIVERSITY
                    EepromSettings.diversityMode = diversity_mode;
                #endif

                #ifdef USE_VOLTAGE_MONITORING
                    EepromSettings.vbatScale = vbat_scale;
                    EepromSettings.vbatWarning = warning_voltage;
                    EepromSettings.vbatCritical = critical_voltage;
                #endif

                drawScreen.save(state_last_used, channelIndex, pgm_read_word_near(channelFreqTable + channelIndex), call_sign);

                EepromSettings.save();

                beep(5);
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
            readVoltage();
            voltageAlarm();
            drawScreen.updateVoltageScreenSaver(voltage, warning_alarm || critical_alarm);
#endif
        do{
            rssi = readRSSI();

#ifdef USE_DIVERSITY
            drawScreen.updateScreenSaver(
                Receiver::activeReceiver,
                rssi,
                Receiver::rssiA,
                Receiver::rssiB
            );
#else
            drawScreen.updateScreenSaver(rssi);
#endif

#ifdef USE_VOLTAGE_MONITORING
            readVoltage();
            voltageAlarm();

            drawScreen.updateVoltageScreenSaver(voltage, warning_alarm || critical_alarm);
#endif
        }
        while((digitalRead(PIN_BUTTON_MODE) == HIGH) && (digitalRead(PIN_BUTTON_UP) == HIGH) && (digitalRead(PIN_BUTTON_DOWN) == HIGH)); // wait for next button press
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
                readVoltage();
                voltageAlarm();
                //delay(100); // timeout delay
            }
            while((digitalRead(PIN_BUTTON_MODE) == HIGH) && (digitalRead(PIN_BUTTON_UP) == HIGH) && (digitalRead(PIN_BUTTON_DOWN) == HIGH)); // wait for next key press

            if(digitalRead(PIN_BUTTON_MODE) == LOW){
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
            } else if(digitalRead(PIN_BUTTON_DOWN) == LOW) {
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
            else if(digitalRead(PIN_BUTTON_UP) == LOW) {
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
            beep();
            do{
                delay(150);// wait for button release
            }
            while(editing==-1 && (digitalRead(PIN_BUTTON_MODE) == LOW || digitalRead(PIN_BUTTON_UP) == LOW || digitalRead(PIN_BUTTON_DOWN) == LOW));
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
                readRSSI();
                drawScreen.updateDiversity(
                    Receiver::activeReceiver,
                    Receiver::rssiA,
                    Receiver::rssiB
                );
            }
            while((digitalRead(PIN_BUTTON_MODE) == HIGH) && (digitalRead(PIN_BUTTON_UP) == HIGH) && (digitalRead(PIN_BUTTON_DOWN) == HIGH)); // wait for next mode or time out

            if(digitalRead(PIN_BUTTON_MODE) == LOW)        // channel UP
            {
                in_menu = 0; // exit menu
            }
            else if(digitalRead(PIN_BUTTON_UP) == LOW) {
                menu_id--;
            }
            else if(digitalRead(PIN_BUTTON_DOWN) == LOW) {
                menu_id++;
            }

            if(menu_id < 0) {
                menu_id = 2;
            }
            if(menu_id > 2) {
                menu_id = 0;
            }

            beep();
            EepromSettings.diversityMode = menu_id;
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
        Receiver::waitForStableRssi();
        rssi = readRSSI();
        rssi_best = (rssi > rssi_best) ? rssi : rssi_best;

        channel=channelFromIndex(channelIndex); // get 0...48 index depending of current channel
        if(state == STATE_MANUAL) // MANUAL MODE
        {
#ifdef USE_IR_EMITTER
            if(time_next_payload+1000 < millis() && rssi <= 50) { // send channel info every second until rssi is locked.
                sendIRPayload();
                time_next_payload = millis();
            }
#endif
            // handling of keys
            if( digitalRead(PIN_BUTTON_UP) == LOW)        // channel UP
            {
                time_screen_saver=millis();
                beep();
                channelIndex++;
                channel++;
                channel > CHANNEL_MAX ? channel = CHANNEL_MIN : false;
                if (channelIndex > CHANNEL_MAX_INDEX)
                {
                    channelIndex = CHANNEL_MIN_INDEX;
                }
            }
            if( digitalRead(PIN_BUTTON_DOWN) == LOW) // channel DOWN
            {
                time_screen_saver=millis();
                beep();
                channelIndex--;
                channel--;
                channel < CHANNEL_MIN ? channel = CHANNEL_MAX : false;
                if (channelIndex > CHANNEL_MAX_INDEX) // negative overflow
                {
                    channelIndex = CHANNEL_MAX_INDEX;
                }
            }

            if(!settings_orderby_channel) { // order by frequency
                channelIndex = pgm_read_byte_near(channelFreqOrderedIndex + channel);
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
                    beep(2);
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
                    channelIndex = pgm_read_byte_near(channelFreqOrderedIndex + channel);
                }
            }
            else
            { // seek was successful

            }
            if (digitalRead(PIN_BUTTON_UP) == LOW || digitalRead(PIN_BUTTON_DOWN) == LOW) // restart seek if key pressed
            {
                if(digitalRead(PIN_BUTTON_UP) == LOW) {
                    seek_direction = 1;
                }
                else {
                    seek_direction = -1;
                }
                beep();
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
        drawScreen.updateSeekMode(state, channelIndex, channel, rssi, pgm_read_word_near(channelFreqTable + channelIndex), rssi_seek_threshold, seek_found);
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
            Receiver::setChannel(channelIndex);
            last_channel_index=channelIndex;
        }

        // print bar for spectrum
        Receiver::waitForStableRssi();
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

                    EepromSettings.rssiAMin = rssi_min_a;
                    EepromSettings.rssiAMax = rssi_max_a;

#ifdef USE_DIVERSITY

                    if(isDiversity()) { // only calibrate RSSI B when diversity is detected.
                        rssi_min_b=rssi_setup_min_b;
                        rssi_max_b=rssi_setup_max_b;
                        if(rssi_max_b < 125) { // user probably did not turn on the VTX during calibration
                            rssi_max_b = RSSI_MAX_VAL;
                        }

                        EepromSettings.rssiBMin = rssi_min_b;
                        EepromSettings.rssiBMax = rssi_max_b;
                    }
#endif
                    state = EepromSettings.defaultState;
                    beep(1, 1000);
                }
            }
        }
        // new scan possible by press scan
        if (digitalRead(PIN_BUTTON_UP) == LOW) // force new full new scan
        {
            beep();
            last_state=255; // force redraw by fake state change ;-)
            channel=CHANNEL_MIN;
            scan_start=1;
            rssi_best=0;
        }
        // update index after channel change
        channelIndex = pgm_read_byte_near(channelFreqOrderedIndex + channel);
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
            while(--in_menu_time_out && ((digitalRead(PIN_BUTTON_MODE) == HIGH) && (digitalRead(PIN_BUTTON_UP) == HIGH) && (digitalRead(PIN_BUTTON_DOWN) == HIGH))) // wait for next key press or time out
            {
                delay(100); // timeout delay
            }

            if(in_menu_time_out <= 0 ) {
                state = state_last_used;
                break; // Timed out, Don't save...
            }

            if(digitalRead(PIN_BUTTON_MODE) == LOW)        // modeButton
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
                            beep();
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
            else if(digitalRead(PIN_BUTTON_UP) == LOW) {
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
            else if(digitalRead(PIN_BUTTON_DOWN) == LOW) {
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

            beep();
            do{
                delay(150);// wait for button release
            }
            while(editing==-1 && (digitalRead(PIN_BUTTON_MODE) == LOW || digitalRead(PIN_BUTTON_UP) == LOW || digitalRead(PIN_BUTTON_DOWN) == LOW));
        }
        while(in_menu);
    }

    /*****************************/
    /*   General house keeping   */
    /*****************************/
    if(last_channel_index != channelIndex)         // tune channel on demand
    {
        Receiver::setChannel(channelIndex);
        last_channel_index=channelIndex;
        // keep time of tune to make sure that RSSI is stable when required
        time_of_tune=millis();
        // give 3 beeps when tuned to give feedback of correct start
        if(first_tune)
        {
            first_tune=0;
            beep(2);
        }
    }
#ifdef USE_VOLTAGE_MONITORING
    readVoltage();
    voltageAlarm();
#endif
}
#endif

/*###########################################################################*/
/*******************/
/*   SUB ROUTINES  */
/*******************/

void beep(uint8_t count, uint16_t milliseconds)
{
    for (uint8_t i = 0; i < count; i++) {
        digitalWrite(PIN_LED, HIGH);
        if (EepromSettings.beepEnabled) {
            digitalWrite(PIN_BUZZER, LOW);
        }

        delay(milliseconds / 2);

        digitalWrite(PIN_LED, LOW);
        if (EepromSettings.beepEnabled) {
            digitalWrite(PIN_BUZZER, HIGH);
        }

        delay(milliseconds / 2);
    }
}

#ifdef OLD_LOOP
uint8_t channelFromIndex(uint8_t channelIndex)
{

    uint8_t loop=0;
    uint8_t channel=0;
    for (loop=0;loop<=CHANNEL_MAX;loop++)
    {
        if(pgm_read_byte_near(channelFreqOrderedIndex + loop) == channelIndex)
        {
            channel=loop;
            break;
        }
    }
    return (channel);
}

uint8_t readRSSI(char receiver)
{
    Receiver::updateRssi();
    #ifdef USE_DIVERSITY
        Receiver::switchDiversity();
    #endif

    #ifdef USE_DIVERSITY
        if (Receiver::activeReceiver == RECEIVER_A || state == STATE_RSSI_SETUP) {
            return Receiver::rssiA;
        }  else {
            return Receiver::rssiB;
        }
    #else
        return Receiver::rssiA;
    #endif
}


#ifdef USE_IR_EMITTER
void sendIRPayload() {
    // beep twice before transmitting.
    beep(2);
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

#ifdef USE_VOLTAGE_MONITORING
void readVoltage()
{
    uint16_t v = analogRead(PIN_VBAT);
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
void voltageAlarm(){
    if(millis() > time_last_vbat_alarm + ALARM_EVERY_MSEC){
        if(critical_alarm){
            //continue playint the critical alarm
            if(millis() - CRITICAL_BEEP_EVERY_MSEC > last_beep){
                //flip the beeper output
                setBuzzer(beeping);
                beeping = !beeping;
                last_beep = millis();
                beep_times++;
            }
            if(beep_times > (CRITICAL_BEEPS*2)) {
                //stop the beeping if we already beeped enough times
                clearAlarm();
                time_last_vbat_alarm = millis();
            }
        } else if(warning_alarm) {
            //continue playint the warning alarm
            if(millis() - WARNING_BEEP_EVERY_MSEC > last_beep){
                //flip the beeper output
                setBuzzer(beeping);
                beeping = !beeping;
                last_beep = millis();
                beep_times++;
            }
            if(beep_times > (WARNING_BEEPS*2)) {
                //stop the beeping if we already beeped enough times
                clearAlarm();
                time_last_vbat_alarm = millis();
            }
        }
    }
}
void clearAlarm(){
    //stop alarm sound when we are at menu etc
    // it might be problematic when were in the middle of a alarm sound
    setBuzzer(false);
    beep_times = 0;
}
void setBuzzer(bool value){
    digitalWrite(PIN_LED, value);
    digitalWrite(PIN_BUZZER, !value);
}
#endif
#endif

void haltWithError() {
    while (true) {
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
        digitalWrite(PIN_BUZZER, !digitalRead(PIN_BUZZER));
        delay(100);
    }
}
