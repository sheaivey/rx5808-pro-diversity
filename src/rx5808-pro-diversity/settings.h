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

#ifndef settings_h
#define settings_h

// Choose the display you will be using
// you will also have to uncomment the includes in the main project.
//#define TVOUT_SCREENS
#define OLED_128x64_ADAFRUIT_SCREENS

// use the library from https://github.com/badzz/Adafruit_SH1106 before enabling
//#define SH1106

// u8glib has performance issues.
//#define OLED_128x64_U8G_SCREENS

// this will be displayed on the screensaver.
// Up to 10 letters
#define CALL_SIGN "CALL SIGN"

// Feature Togglels
#define USE_DIVERSITY
#define USE_IR_EMITTER
//#define USE_FLIP_SCREEN
#define USE_BOOT_LOGO
// you can use any of the arduino analog pins to measure the voltage of the battery
// TODO: interface works only for TVOUT_SCREENS
//#define USE_VOLTAGE_MONITORING
// Choose if you wish to use 8 additional Channels
// 5362 MHz 5399 MHz 5436 MHz 5473 MHz 5510 MHz 5547 MHz 5584 MHz 5621 MHz
// Local laws may prohibit the use of these frequencies use at your own risk!
//#define USE_LBAND

// Receiver Module version
// used for tuning time
#define rx5808
//#define rx5880


#define spiDataPin 10
#define slaveSelectPin 11
#define spiClockPin 12

// Receiver PINS
#define receiverA_led A0
#define rssiPinA A6

#define useReceiverA 1

#ifdef USE_DIVERSITY
    // Diversity
    #define receiverB_led A1
    #define rssiPinB A7
    #define useReceiverAuto 0
    #define useReceiverB 2
    // rssi strenth should be 2% greater than other receiver before switch.
    // this pervents flicker when rssi values are close and delays diversity checks counter.
    #define DIVERSITY_CUTOVER 2
    // number of checks a receiver needs to win over the other to switch receivers.
    // this pervents rapid switching.
    // 1 to 10 is a good range. 1 being fast switching, 10 being slow 100ms to switch.
    #define DIVERSITY_MAX_CHECKS 5
#endif

#ifdef USE_VOLTAGE_MONITORING
    // Voltage monitoring
    // you can use any arduino analog input to measure battery voltage
    // keep in mind that A4 and A5 is used by OLED and A6 and A7 are used for measuring RSSI
    // use a voltage divider to lower the voltage to max 5V - values for max  13V (3s)
    // You can use a 100nF capacitor near the arduino pin to smooth the voltage
    //
    //           R1 = 5.6k
    //    BAT+ ----====----+----+---- ARDUINO ANALOG PIN
    //                     |    |
    //                     |    |  (optional)
    //                     |    || 100n CAP
    //                     |    |
    //           R2 = 3.3k |    |
    //    BAT- ----====----|----|

    #ifdef TVOUT_SCREENS
        #define VBAT_PIN A4
    #else
        #define VBAT_PIN A0
    #endif

    // these are default values
    #define WARNING_VOLTAGE 108 // 3.6V per cell for 3S
    #define CRITICAL_VOLTAGE 100 // 3.3V per cell for 3S
    #define VBAT_SCALE 119
    #define VBAT_OFFSET 0
#endif

// this two are minimum required
#define buttonUp 2
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
// number of analog rssi reads to average for the current check.
#define RSSI_READS 50
// RSSI default raw range
#define RSSI_MIN_VAL 90
#define RSSI_MAX_VAL 220
// 75% threshold, when channel is printed in spectrum
#define RSSI_SEEK_FOUND 75
// 80% under max value for RSSI
#define RSSI_SEEK_TRESHOLD 80
// scan loops for setup run
#define RSSI_SETUP_RUN 3

#define STATE_SEEK_FOUND 0
#define STATE_SEEK 1
#define STATE_SCAN 2
#define STATE_MANUAL 3
#ifdef USE_DIVERSITY
    #define STATE_DIVERSITY 4
#endif
#define STATE_SETUP_MENU 5
#define STATE_SAVE 6
#define STATE_RSSI_SETUP 7
#define STATE_SCREEN_SAVER 8
#define STATE_VOLTAGE 9

// Seconds to wait before force entering screensaver
#define SCREENSAVER_TIMEOUT 30

#define START_STATE STATE_SEEK
#define MAX_STATE STATE_MANUAL
#ifdef USE_VOLTAGE_MONITORING
    #define SETUP_MENU_MAX_ITEMS 5
#else
    #define SETUP_MENU_MAX_ITEMS 4
#endif

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN_INDEX 0
#ifdef USE_LBAND
    #define CHANNEL_MAX_INDEX 47
#else
    #define CHANNEL_MAX_INDEX 39
#endif

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

#ifdef USE_LBAND
    #define CHANNEL_MAX 47
#else
    #define CHANNEL_MAX 39
#endif
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

    // used to figure out if diversity module has been plugged in.
    // When RSSI is plugged in the min value is around 90
    // When RSSI is not plugged in the min value is 0
    #define isDiversity() (analogRead(rssiPinB) >= 5)
#endif

#define EEPROM_ADR_BEEP 11
#define EEPROM_ADR_ORDERBY 12

#ifdef USE_VOLTAGE_MONITORING
    #define EEPROM_ADR_VBAT_SCALE 13
    #define EEPROM_ADR_VBAT_WARNING 14
    #define EEPROM_ADR_VBAT_CRITICAL 15
#endif

#define EEPROM_ADR_CALLSIGN 20

#endif // file_defined
