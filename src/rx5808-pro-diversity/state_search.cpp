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
    0x00, 0x00, 0x1E, 0x00, 0x3F, 0x00, 0x73, 0x80, 0x61, 0x98, 0x7F, 0x84, 0x7F, 0x82, 0x61, 0x82,
    0x61, 0x80, 0x61, 0x80, 0x61, 0xA2, 0x08, 0x36, 0x08, 0x2A, 0x04, 0x22, 0x03, 0x22, 0x00, 0x00
};

const unsigned char manualIcon[] PROGMEM = {
    0x00, 0x00, 0x60, 0xC0, 0x71, 0xC0, 0x7B, 0xC0, 0x7F, 0xD8, 0x7F, 0xC4, 0x6E, 0xC2, 0x64, 0xC2,
    0x60, 0xC0, 0x60, 0xC0, 0x60, 0xC0, 0x08, 0x0C, 0x08, 0x12, 0x04, 0x1E, 0x03, 0x12, 0x00, 0x00
};

const unsigned char channelOrderIcon[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x33, 0x9C, 0x4A, 0x52, 0x7B, 0x9C, 0x4A, 0x52, 0x4B, 0x92, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x08, 0x30, 0x0C, 0x7E, 0x7E, 0x30, 0x0C, 0x10, 0x08, 0x00, 0x00
};

const unsigned char freqOrderIcon[] PROGMEM = {
    0x00, 0x00, 0x19, 0x80, 0x19, 0x80, 0x19, 0x80, 0x1F, 0xB8, 0x1F, 0x88, 0x19, 0x90, 0x19, 0xA0,
    0x19, 0xB8, 0x00, 0x00, 0x10, 0x08, 0x30, 0x0C, 0x7E, 0x7E, 0x30, 0x0C, 0x10, 0x08, 0x00, 0x00
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

    EepromSettings.searchManual = search->manual;
    EepromSettings.markDirty();
}

static void menuOrderHandler(void* state) {
    SearchStateHandler* search = static_cast<SearchStateHandler*>(state);
    if (search->order == SearchStateHandler::ScanOrder::FREQUENCY) {
        search->order = SearchStateHandler::ScanOrder::CHANNEL;
        search->orderedChanelIndex =
            Channels::getOrderedIndex(search->orderedChanelIndex);
        EepromSettings.searchOrderByChannel = true;
    } else {
        search->order = SearchStateHandler::ScanOrder::FREQUENCY;
        search->orderedChanelIndex =
            Channels::getOrderedIndexFromIndex(search->orderedChanelIndex);
        EepromSettings.searchOrderByChannel = false;
    }

    EepromSettings.markDirty();
}


void SearchStateHandler::onEnter() {
    menu.addItem(menuModeIcon, menuModeHandler);
    menu.addItem(menuOrderIcon, menuOrderHandler);

    this->manual = EepromSettings.searchManual;
    this->order = EepromSettings.searchOrderByChannel ?
        ScanOrder::CHANNEL :
        ScanOrder::FREQUENCY;

    switch (this->order) {
        case ScanOrder::CHANNEL:
            this->orderedChanelIndex = EepromSettings.startChannel;
            break;

        case ScanOrder::FREQUENCY:
            this->orderedChanelIndex =
                Channels::getOrderedIndexFromIndex(EepromSettings.startChannel);
            break;
    }
}

void SearchStateHandler::onUpdate() {
    if (!manual) {
        onUpdateAuto();
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

            EepromSettings.startChannel =
                Channels::getOrderedIndex(peakChannel);
            EepromSettings.markDirty();

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

void SearchStateHandler::onButtonChange(
    Button button,
    Buttons::PressType pressType
) {
    if (this->menu.handleButtons(button, pressType))
        return;

    if (!this->manual) {
        if (
            pressType != Buttons::PressType::SHORT ||
            button == Button::MODE
        ) {
            return;
        }

        scanning = true;
        forceNext = true;
        direction = button == Button::UP ?
            ScanDirection::UP : ScanDirection::DOWN;
    } else {
        if (
            pressType != Buttons::PressType::SHORT &&
            pressType != Buttons::PressType::HOLDING
        ) {
            return;
        }

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
    EepromSettings.startChannel = actualChannelIndex;
    EepromSettings.markDirty();
}
