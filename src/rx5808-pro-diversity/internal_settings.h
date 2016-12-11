/*
 * Setings file by Shea Ivey

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

#ifndef INTERNAL_SETTINGS_H
#define INTERNAL_SETTINGS_H


#include "settings.h"

// === State ===================================================================

#define STATE_SEEK_FOUND 0
#define STATE_SEEK 1
#define STATE_SCAN 2
#define STATE_MANUAL 3
#define STATE_SETUP_MENU 4
#define STATE_SAVE 5
#define STATE_RSSI_SETUP 6
#define STATE_SCREEN_SAVER 7
#ifdef USE_DIVERSITY
    #define STATE_DIVERSITY 8
#endif
#ifdef USE_VOLTAGE_MONITORING
    #define STATE_VOLTAGE 9
#endif

#define START_STATE STATE_SEEK
#define MAX_STATE STATE_MANUAL

// === Setup Menu ==============================================================

#ifdef USE_VOLTAGE_MONITORING
    #define SETUP_MENU_MAX_ITEMS 5
#else
    #define SETUP_MENU_MAX_ITEMS 4
#endif

// === Channels ================================================================

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN 0
#define CHANNEL_MIN_INDEX 0
#ifdef USE_LBAND
    #define CHANNEL_MAX 47
    #define CHANNEL_MAX_INDEX 47
#else
    #define CHANNEL_MAX 39
    #define CHANNEL_MAX_INDEX 39
#endif

// === EEPROM ==================================================================

#define EEPROM_MAGIC 0x1337

#define EEPROM_ADR_MAGIC_L 0
#define EEPROM_ADR_MAGIC_H 1
#define EEPROM_ADR_STATE 2
#define EEPROM_ADR_TUNE 3
#define EEPROM_ADR_RSSI_MIN_A_L 4
#define EEPROM_ADR_RSSI_MIN_A_H 5
#define EEPROM_ADR_RSSI_MAX_A_L 6
#define EEPROM_ADR_RSSI_MAX_A_H 7
#ifdef USE_DIVERSITY
    #define EEPROM_ADR_DIVERSITY 8
    #define EEPROM_ADR_RSSI_MIN_B_L 9
    #define EEPROM_ADR_RSSI_MIN_B_H 10
    #define EEPROM_ADR_RSSI_MAX_B_L 11
    #define EEPROM_ADR_RSSI_MAX_B_H 12

    // Used to figure out if diversity module has been plugged in.
    // When RSSI is plugged in the min value is around 90.
    // When RSSI is not plugged in the min value is 0.
    #define isDiversity() (analogRead(PIN_RSSI_B) >= 5)
#endif

#define EEPROM_ADR_BEEP 13
#define EEPROM_ADR_ORDERBY 14
#define EEPROM_ADR_CALLSIGN 20

#ifdef USE_VOLTAGE_MONITORING
    #define EEPROM_ADR_VBAT_SCALE 15
    #define EEPROM_ADR_VBAT_WARNING 16
    #define EEPROM_ADR_VBAT_CRITICAL 17
#endif

// === Receiver Modules =========================================================

#ifdef rx5808
    // rx5808 module need >20ms to tune.
    // 25 ms will do a 40 channel scan in 1 second.
    #define MIN_TUNE_TIME 25
#endif
#ifdef rx5880
    // rx5880 module needs >30ms to tune.
    // 35 ms will do a 40 channel scan in 1.4 seconds.
    #define MIN_TUNE_TIME 35
#endif

#ifdef USE_DIVERSITY
    #define useReceiverAuto 0
    #define useReceiverB 2
#endif

// === Display Modules =========================================================

#ifdef SH1106
  #define OLED_VCCSTATE SH1106_SWITCHCAPVCC
  #define OLED_CLASS Adafruit_SH1106
#else
  #define OLED_VCCSTATE SSD1306_SWITCHCAPVCC
  #define OLED_CLASS Adafruit_SSD1306
#endif

// === Misc ====================================================================

#ifdef USE_VOLTAGE_MONITORING
    #define VBAT_SMOOTH 8
    #define VBAT_PRESCALER 16
#endif

#endif // file_defined