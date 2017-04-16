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


const unsigned char autoIcon[] PROGMEM = {
    0x00, 0x00, 0x1F, 0x00, 0x3F, 0x80, 0x71, 0xC0, 0x60, 0xC0, 0x60, 0xC0, 0x60, 0xC0, 0x60, 0xC0,
    0x7F, 0xD2, 0x7F, 0xD2, 0x60, 0xD2, 0x60, 0xD2, 0x60, 0xD2, 0x60, 0xD2, 0x60, 0xCC, 0x00, 0x00
};

const unsigned char manualIcon[] PROGMEM = {
    0x00, 0x00, 0x20, 0x80, 0x71, 0xC0, 0x7B, 0xC0, 0x7F, 0xC0, 0x6E, 0xC0, 0x6E, 0xC0, 0x64, 0xC0,
    0x60, 0xCC, 0x60, 0xD2, 0x60, 0xD2, 0x60, 0xDE, 0x60, 0xD2, 0x60, 0xD2, 0x60, 0xD2, 0x00, 0x00
};

const unsigned char channelOrderIcon[] PROGMEM = {
    0x00, 0x00, 0x26, 0x08, 0x55, 0x08, 0x56, 0x1C, 0x75, 0x1C, 0x55, 0x3E, 0x56, 0x3E, 0x00, 0x00,
    0x00, 0x00, 0x77, 0x3E, 0x44, 0x3E, 0x77, 0x1C, 0x44, 0x1C, 0x44, 0x08, 0x74, 0x08, 0x00, 0x00
};

const unsigned char freqOrderIcon[] PROGMEM = {
    0x00, 0x00, 0x48, 0x08, 0x48, 0x08, 0x48, 0x1C, 0x48, 0x1C, 0x48, 0x3E, 0x48, 0x3E, 0x48, 0x00,
    0x7B, 0x80, 0x48, 0xBE, 0x49, 0x3E, 0x49, 0x1C, 0x4A, 0x1C, 0x4A, 0x08, 0x4B, 0x88, 0x00, 0x00
};


static const unsigned char* menuModeIcon(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    return search->manual ? manualIcon : autoIcon;
}

static const unsigned char* menuOrderIcon(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    switch (search->order) {
        case SearchStateHandler::ScanOrder::FREQUENCY:
            return freqOrderIcon;
            break;

        case SearchStateHandler::ScanOrder::CHANNEL:
            return channelOrderIcon;
            break;
    }
}

static void menuModeHandler(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    search->manual = !search->manual;
}

static void menuOrderHandler(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    if (search->order == SearchStateHandler::ScanOrder::FREQUENCY) {
        search->order = SearchStateHandler::ScanOrder::CHANNEL;
    } else {
        search->order = SearchStateHandler::ScanOrder::FREQUENCY;
    }
}


void SearchStateHandler::onEnter() {
    menu.addItem(menuModeIcon, menuModeHandler);
    menu.addItem(menuOrderIcon, menuOrderHandler);
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

            this->setChannel();
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

        this->setChannel();
    }
}

void SearchStateHandler::setChannel() {
    uint8_t actualChannelIndex;
    if (this->order == ScanOrder::FREQUENCY) {
        actualChannelIndex = Channels::getOrderedIndex(orderedChanelIndex);
    } else {
        actualChannelIndex = orderedChanelIndex;
    }

    Receiver::setChannel(actualChannelIndex);
}
