#include <Arduino.h>

#include "state_search.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "buttons.h"
#include "ui.h"
#include "pstr_helper.h"


using StateMachine::SearchStateHandler;


static char* menuModeText(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    return search->manual ? PSTR2("M") : PSTR2("A");
}

static char* menuOrderText(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    return PSTR2("F");
}

static void menuModeHandler(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    search->manual = !search->manual;
}

static void menuOrderHandler(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    return;
}


void SearchStateHandler::onEnter() {
    menu.addItem(
        menuModeText,
        menuModeHandler
    );

    menu.addItem(
        menuOrderText,
        menuOrderHandler
    );
}

void SearchStateHandler::onUpdate() {
    Receiver::waitForStableRssi();

    if (!manual) {
        onUpdateAuto();
    } else {
        onUpdateManual();
    }

    Ui::needUpdate();
}

void SearchStateHandler::onUpdateAuto() {
    if (scanningPeak) {
        uint8_t peaksIndex = peakChannelIndex - orderedChanelIndex;
        peaks[peaksIndex] = Receiver::rssiA;
        peakChannelIndex++;

        if (peaksIndex >= PEAK_LOOKAHEAD || peakChannelIndex >= CHANNELS_SIZE) {
            uint8_t largestPeak = 0;
            uint8_t largestPeakIndex = 0;
            for (uint8_t i = 0; i < PEAK_LOOKAHEAD; i++) {
                uint8_t peak = peaks[i];
                if (peak > largestPeak) {
                    largestPeak = peak;
                    largestPeakIndex = i;
                }
            }

            uint8_t peakChannel = orderedChanelIndex + largestPeakIndex;
            orderedChanelIndex = peakChannel;
            Receiver::setChannel(Channels::getOrderedIndex(peakChannel));

            scanningPeak = false;
        } else {
            Receiver::setChannel(Channels::getOrderedIndex(peakChannelIndex));
        }
    } else {
        if (scanning) {
            if (!forceNext && Receiver::rssiA >= RSSI_SEEK_TRESHOLD) {
                scanning = false;
                scanningPeak = true;
                peakChannelIndex = orderedChanelIndex;

                for (uint8_t i = 0; i < PEAK_LOOKAHEAD; i++)
                    peaks[i] = 0;
            } else {
                orderedChanelIndex += static_cast<int8_t>(direction);
                if (orderedChanelIndex == 255)
                    orderedChanelIndex = CHANNELS_SIZE - 1;
                else if (orderedChanelIndex >= CHANNELS_SIZE)
                    orderedChanelIndex = 0;

                Receiver::setChannel(
                    Channels::getOrderedIndex(orderedChanelIndex));

                if (forceNext)
                    forceNext = false;
            }
        }
    }
}

void SearchStateHandler::onUpdateManual() {

}

void SearchStateHandler::onButtonChange(
    Button button,
    Buttons::PressType pressType
) {
    if (this->menu.handleButtons(button, pressType))
        return;

    if (pressType == Buttons::PressType::SHORT) {
        if (manual) {
            if (button == Button::UP) {
                orderedChanelIndex += 1;
            } else if (button == Button::DOWN) {
                orderedChanelIndex -= 1;
            }

            if (orderedChanelIndex == 255)
                orderedChanelIndex = CHANNELS_SIZE - 1;
            else if (orderedChanelIndex >= CHANNELS_SIZE)
                orderedChanelIndex = 0;

            Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));
        } else {
            if (button == Button::UP) {
                scanning = true;
                forceNext = true;
                direction = ScanDirection::UP;
            } else if (button == Button::DOWN) {
                scanning = true;
                forceNext = true;
                direction = ScanDirection::DOWN;
            }
        }
    } else if (pressType == Buttons::PressType::HOLDING) {
        if (button == Button::UP) {
            orderedChanelIndex += 1;
        }

        if (button == Button::DOWN) {
            orderedChanelIndex -= 1;
        }

        if (orderedChanelIndex == 255)
            orderedChanelIndex = CHANNELS_SIZE - 1;
        else if (orderedChanelIndex >= CHANNELS_SIZE)
            orderedChanelIndex = 0;

        Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));
    }
}
