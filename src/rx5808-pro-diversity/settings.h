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
//#define TVOUT_SCREENS
#define OLED_128x64_ADAFRUIT_SCREENS
// u8glib has performance issues.
//#define OLED_128x64_U8G_SCREENS

#ifdef TVOUT_SCREENS
    #include "TVout.h"
    #include "fontALL.h"
	#define MAX_RXS 2
#endif
#ifdef OLED_128x64_ADAFRUIT_SCREENS
    #include <Adafruit_SSD1306.h>
    #include <Adafruit_GFX.h>
    #include <Wire.h>
    #include <SPI.h>
	#define MAX_RXS 6
#endif
#ifdef OLED_128x64_U8G_SCREENS
    #include <U8glib.h>
	#define MAX_RXS 1
#endif


// Up to 10 letters
#define CALL_SIGN "CALL SIGN"
// Include if call sign to be displayed on the screensaver (reduces max number of receivers)
#define DISPLAY_CALL_SIGN 1


// Feature Toggles
//#define USE_IR_EMITTER
//#define USE_FLIP_SCREEN
//#define USE_BOOT_LOGO

// Receiver Module version
// used for tuning time
//#define rx5808
#define rx5880

//#define NUM_RXS 3
#define NUM_RXS 4

#if (NUM_RXS >= MAX_RXS)
 #undef NUM_RXS
 #define NUM_RXS MAX_RXS
 #ifdef DISPLAY_CALL_SIGN
  #ifdef OLED_128x64_ADAFRUIT_SCREENS
   #undef NUM_RXS 
   #define NUM_RXS (MAX_RXS-1)
  #endif
 #endif
#endif


//RSSI pins - The following must include a pin ID of the rssi input & receiver video switch output for each receiver
//#define SET_RSSI_PINS         const uint8_t rssi_pins[] = {A6,A7,A3} 
#define SET_RSSI_PINS         const uint8_t rssi_pins[] = {A2,A3,A6,A7} 

//SPI Pins
#define spiDataPin 10
#define slaveSelectPin 11
#define spiClockPin 12

// Receiver Pins - The following must include a pin ID of the receiver video switch output for each receiver
//#define SET_RECEIVER_LED_PINS const uint8_t receiverLEDPins[] = {A0,A1,A2}
#define SET_RECEIVER_LED_PINS const uint8_t receiverLEDPins[] = {9,8,7,6}

// Auto & Rx A constants
#define useReceiverAuto NUM_RXS
#define useReceiverA 0

//Button pins
// These two are minimum buttons required
#define buttonUp 2
#define buttonMode 3
// Optional comfort buttons - comment out if not using
#define buttonDown 4
//#define buttonSave 5
// Buzzer - comment out if not using
//#define buzzer 5

//LED pin
#define led 13


// rssi strength should be 2% greater than other receiver before switch -
// this prevents flicker when RSSI values are close and delays diversity checks counter.
#define DIVERSITY_CUTOVER 2
// number of checks a receiver needs to win over the other to switch receivers -  this prevents rapid switching.
// 1 to 10 is a good range. 1 being fast switching, 10 being slow 100ms to switch.
#define DIVERSITY_MAX_CHECKS 5

// number of analog rssi reads to average for the current check.
#define RSSI_READS 50
// RSSI default raw range
#define RSSI_MIN_VAL 90
#define RSSI_MAX_VAL 220
// 75% threshold, when channel is printed in spectrum
#define RSSI_SEEK_FOUND 75
// scan loops for setup run
#define RSSI_SETUP_RUN 3
//Additional % overhead included following calibration
#define RSSI_ADDITIONAL_HEADROOM 5
// 80% under max value for RSSI
#define RSSI_SEEK_THRESHOLD (80*100)/(100+RSSI_ADDITIONAL_HEADROOM)

#ifdef rx5808
    // rx5808 module need >20ms to tune.
    // 25 ms will do a 40 channel scan in 1 second.
    #define MIN_TUNE_TIME 25
#endif
#ifdef rx5880
    // rx5880 module needs >30ms to tune.
    // 35 ms will do a 40 channel scan in 1.4 seconds.
    #define MIN_TUNE_TIME 40
#endif

// Key debounce delay in ms
// shorter values will make it more reactive, but may lead to double trigger
// NOTE: good values are in the range of 100-200ms
#define KEY_DEBOUNCE 200

// Seconds to wait before force entering screensaver
#define SCREENSAVER_TIMEOUT 10

// Seconds to display calibration results for
#define CALIBRATION_RESULTS_TIME 15


#define STATE_SEEK_FOUND 0
#define STATE_SEEK 1
#define STATE_SCAN 2
#define STATE_MANUAL 3
#define STATE_DIVERSITY 4
#define STATE_SETUP_MENU 5
#define STATE_SAVE 6
#define STATE_RSSI_SETUP 7
#define STATE_SCREEN_SAVER 8
#define START_STATE STATE_SEEK
//#define MAX_STATE STATE_MANUAL  - Dont think this is used anymore

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN_INDEX 0
#define CHANNEL_MAX_INDEX 39
#define CHANNEL_MAX 39
#define CHANNEL_MIN 0

#define EEPROM_ADR_STATE 0
#define EEPROM_ADR_TUNE 1
#define EEPROM_ADR_BEEP 2
#define EEPROM_ADR_ORDERBY 3
#define EEPROM_ADR_DIVERSITY 4
#define EEPROM_ADR_RSSI_MIN 5
#define EEPROM_ADR_RSSI_MIN 6
#define EEPROM_ADR_RSSI_MAX 7
#define EEPROM_ADR_RSSI_MAX 8
#define EEPROM_ADR_CALLSIGN 30

#endif // file_defined
