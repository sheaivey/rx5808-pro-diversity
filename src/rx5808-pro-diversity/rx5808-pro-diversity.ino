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

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"

#include "channels.h"
#include "receiver.h"
#include "receiver_spi.h"
#include "buttons.h"
#include "state.h"
#include "ui.h"


static void globalMenuButtonHandler(
    Button button,
    Buttons::PressType pressType
);


void setup()
{
    setupPins();

    // Enable buzzer and LED for duration of setup process.
    digitalWrite(PIN_LED, HIGH);
    digitalWrite(PIN_BUZZER, LOW);

    setupSettings();

    StateMachine::setup();
    Receiver::setup();
    Ui::setup();

    Receiver::setActiveReceiver(Receiver::ReceiverId::A);

    #ifdef USE_IR_EMITTER
        Serial.begin(9600);
    #endif
    #ifdef USE_SERIAL_OUT
        Serial.begin(250000);
    #endif

    // Setup complete.
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_BUZZER, HIGH);

    Buttons::registerChangeFunc(globalMenuButtonHandler);

    // Switch to initial state.
    StateMachine::switchState(StateMachine::State::SEARCH);
}

void setupPins() {
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_BUTTON_UP_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_MODE_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_SAVE_PRESSED, INPUT_PULLUP);

    pinMode(PIN_LED_A,OUTPUT);
    #ifdef USE_DIVERSITY
        pinMode(PIN_LED_B,OUTPUT);
    #endif

    pinMode(PIN_RSSI_A, INPUT_PULLUP);
    #ifdef USE_DIVERSITY
        pinMode(PIN_RSSI_B, INPUT_PULLUP);
    #endif

    pinMode(PIN_SPI_SLAVE_SELECT, OUTPUT);
    pinMode(PIN_SPI_DATA, OUTPUT);
    pinMode(PIN_SPI_CLOCK, OUTPUT);

    digitalWrite(PIN_SPI_SLAVE_SELECT, HIGH);
    digitalWrite(PIN_SPI_CLOCK, LOW);
    digitalWrite(PIN_SPI_DATA, LOW);
}

void setupSettings() {
    EepromSettings.load();
    Receiver::setChannel(EepromSettings.startChannel);
}


void loop() {
    Receiver::update();
    Buttons::update();
    StateMachine::update();
    Ui::update();
    EepromSettings.update();

    if (
        StateMachine::currentState != StateMachine::State::SCREENSAVER
        && StateMachine::currentState != StateMachine::State::BANDSCAN
        && (millis() - Buttons::lastChangeTime) >
            (SCREENSAVER_TIMEOUT * 1000)
    ) {
        StateMachine::switchState(StateMachine::State::SCREENSAVER);
    }
}


static void globalMenuButtonHandler(
    Button button,
    Buttons::PressType pressType
) {
    if (
        StateMachine::currentState != StateMachine::State::MENU &&
        button == Button::MODE_PRESSED &&
        pressType == Buttons::PressType::HOLDING
    ) {
        StateMachine::switchState(StateMachine::State::MENU);
    }
}
