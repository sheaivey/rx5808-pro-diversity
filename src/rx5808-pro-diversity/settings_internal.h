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

// === EEPROM ==================================================================

// This should be incremented after every EEPROM change.
#define EEPROM_MAGIC 0x00000009

// === Receiver Modules =========================================================

#ifdef RX5808
    // rx5808 module need >20ms to tune.
    // 25 ms will do a 40 channel scan in 1 second.
    #define MIN_TUNE_TIME 25
#endif

#ifdef RX5880
    // rx5880 module needs >30ms to tune.
    // 35 ms will do a 40 channel scan in 1.4 seconds.
    #define MIN_TUNE_TIME 35
#endif

// === Display Modules =========================================================

#ifdef SH1106
  #define OLED_VCCSTATE SH1106_SWITCHCAPVCC
  #define OLED_CLASS Adafruit_SH1106
#else
  #define OLED_VCCSTATE SSD1306_SWITCHCAPVCC
  #define OLED_CLASS Adafruit_SSD1306
#endif

#define OLED_FRAMERATE 1000 / 25

// === Misc ====================================================================

#ifdef USE_VOLTAGE_MONITORING
    #define VBAT_SMOOTH 8
    #define VBAT_PRESCALER 16
#endif

#define EEPROM_SAVE_TIME 5000

#endif // file_defined
