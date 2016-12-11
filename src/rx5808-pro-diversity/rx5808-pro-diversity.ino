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
#include "internal_settings.h"
#include "eeprom_settings.h"

#include "screens.h"
#include "channels.h"

screens drawScreen;

char channel = 0;
uint8_t channelIndex = 0;
uint8_t rssi = 0;
uint8_t active_receiver = useReceiverA;
#ifdef USE_DIVERSITY
    uint8_t diversity_mode = useReceiverAuto;
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

// SETUP ----------------------------------------------------------------------------
void setup()
{
    // IO INIT
    // initialize digital pin 13 LED as an output.
    pinMode(PIN_LED, OUTPUT); // status pin for TV mode errors
    digitalWrite(PIN_LED, HIGH);
    // buzzer
    pinMode(PIN_BUZZER, OUTPUT); // Feedback buzzer (active buzzer, not passive piezo)
    digitalWrite(PIN_BUZZER, HIGH);
    // minimum control pins
    pinMode(PIN_BUTTON_UP, INPUT);
    digitalWrite(PIN_BUTTON_UP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_MODE, INPUT);
    digitalWrite(PIN_BUTTON_MODE, INPUT_PULLUP);
    // optional control
    pinMode(PIN_BUTTON_DOWN, INPUT);
    digitalWrite(PIN_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_SAVE, INPUT);
    digitalWrite(PIN_BUTTON_SAVE, INPUT_PULLUP);
    //Receiver Setup
    pinMode(PIN_LED_A,OUTPUT);
#ifdef USE_DIVERSITY
    pinMode(PIN_LED_B,OUTPUT);
#endif
    setReceiver(useReceiverA);
    // SPI pins for RX control
    pinMode (PIN_SPI_SLAVE_SELECT, OUTPUT);
    pinMode (PIN_SPI_DATA, OUTPUT);
	pinMode (PIN_SPI_CLOCK, OUTPUT);

    EepromSettings.load();

    // read last setting from eeprom
    state = EepromSettings.defaultState;
    channelIndex = EepromSettings.channel;

    // set the channel as soon as we can
    // faster boot up times :)
    setChannelModule(channelIndex);
    last_channel_index=channelIndex;

    settings_beeps = EepromSettings.beepEnabled;
    settings_orderby_channel = EepromSettings.orderByChannel;

    // load saved call sign
    memcpy(call_sign, EepromSettings.callSign, sizeof(call_sign));

    rssi_min_a = EepromSettings.rssiAMin;
    rssi_max_a = EepromSettings.rssiAMax;
#ifdef USE_DIVERSITY
    diversity_mode = EepromSettings.diversityMode;
    rssi_min_b = EepromSettings.rssiBMin;
    rssi_max_b = EepromSettings.rssiBMax;
#endif
    force_menu_redraw=1;

    // Init Display
    if (drawScreen.begin(call_sign) > 0) {
        // on Error flicker LED
        while (true) { // stay in ERROR for ever
            digitalWrite(PIN_LED, !digitalRead(PIN_LED));
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
        vbat_scale = EepromSettings.vbatScale;
        warning_voltage = EepromSettings.vbatWarning;
        critical_voltage = EepromSettings.vbatCritical;
#endif
    // Setup Done - Turn Status LED off.
    digitalWrite(PIN_LED, LOW);

}


// LOOP ----------------------------------------------------------------------------
void loop()
{
    /*******************/
    /*   Mode Select   */
    /*******************/
    uint8_t in_menu;
    uint8_t in_menu_time_out;

    if (digitalRead(PIN_BUTTON_MODE) == LOW) // key pressed ?
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
        while(digitalRead(PIN_BUTTON_MODE) == LOW && press_time < 10)
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
    if(digitalRead(PIN_BUTTON_SAVE) == LOW)
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
                read_voltage();
                voltage_alarm();
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
            beep(50); // beep & debounce
            //delay(KEY_DEBOUNCE); // debounce
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
                //delay(10); // timeout delay
                readRSSI();
                drawScreen.updateDiversity(active_receiver, readRSSI(useReceiverA), readRSSI(useReceiverB));
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
            if( digitalRead(PIN_BUTTON_UP) == LOW)        // channel UP
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
            if( digitalRead(PIN_BUTTON_DOWN) == LOW) // channel DOWN
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
            if (digitalRead(PIN_BUTTON_UP) == LOW || digitalRead(PIN_BUTTON_DOWN) == LOW) // restart seek if key pressed
            {
                if(digitalRead(PIN_BUTTON_UP) == LOW) {
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
                    beep(1000);
                }
            }
        }
        // new scan possible by press scan
        if (digitalRead(PIN_BUTTON_UP) == LOW) // force new full new scan
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

            beep(50); // beep & debounce
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
    digitalWrite(PIN_LED, HIGH);
    if(settings_beeps){
        digitalWrite(PIN_BUZZER, LOW); // activate beep
    }
    delay(time/2);
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_BUZZER, HIGH);
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
        rssiA += analogRead(PIN_RSSI_A);//random(RSSI_MAX_VAL-200, RSSI_MAX_VAL);//

#ifdef USE_DIVERSITY
        rssiB += analogRead(PIN_RSSI_B);//random(RSSI_MAX_VAL-200, RSSI_MAX_VAL);//
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
        digitalWrite(PIN_LED_B, LOW);
        digitalWrite(PIN_LED_A, HIGH);
    }
    else
    {
        digitalWrite(PIN_LED_A, LOW);
        digitalWrite(PIN_LED_B, HIGH);
    }
#else
    digitalWrite(PIN_LED_A, HIGH);
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

  digitalWrite(PIN_SPI_SLAVE_SELECT, LOW);
  digitalWrite(PIN_SPI_CLOCK, LOW);
  digitalWrite(PIN_SPI_DATA, LOW);
}


void SERIAL_SENDBIT1()
{
  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_DATA, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_CLOCK, HIGH);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);
}

void SERIAL_SENDBIT0()
{
  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_DATA, LOW);
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_CLOCK, HIGH);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);
}

void SERIAL_ENABLE_LOW()
{
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_SLAVE_SELECT, LOW);
  delayMicroseconds(1);
}

void SERIAL_ENABLE_HIGH()
{
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_SLAVE_SELECT, HIGH);
  delayMicroseconds(1);
}


#ifdef USE_VOLTAGE_MONITORING
void read_voltage()
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
void set_buzzer(bool value){
    digitalWrite(PIN_LED, value);
    digitalWrite(PIN_BUZZER, !value);
}
#endif
