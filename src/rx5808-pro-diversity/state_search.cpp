#include <Arduino.h>

#include "state_search.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "buttons.h"
#include "ui.h"


void StateMachine::SearchStateHandler::onUpdate() {
    Receiver::waitForStableRssi();

    if (!manual) {
        onUpdateAuto();
    } else {
        onUpdateManual();
    }

    Ui::needUpdate();
}

void StateMachine::SearchStateHandler::onUpdateAuto() {
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

void StateMachine::SearchStateHandler::onUpdateManual() {
    if (
        Buttons::get(Button::UP)->pressed &&
        millis() - Buttons::get(Button::UP)->pressTime > 500
    ) {
        orderedChanelIndex += 1;
    }

    if (
        Buttons::get(Button::DOWN)->pressed &&
        millis() - Buttons::get(Button::DOWN)->pressTime > 500
    ) {
        orderedChanelIndex -= 1;
    }

    if (orderedChanelIndex == 255)
        orderedChanelIndex = CHANNELS_SIZE - 1;
    else if (orderedChanelIndex >= CHANNELS_SIZE)
        orderedChanelIndex = 0;

    Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));
}

void StateMachine::SearchStateHandler::onButtonChange() {
    if (Buttons::get(Button::MODE)->pressed) {
        manual = !manual;
    }

    if (manual) {
        if (Buttons::get(Button::UP)->pressed) {
            orderedChanelIndex += 1;
        } else if (Buttons::get(Button::DOWN)->pressed) {
            orderedChanelIndex -= 1;
        }

        if (orderedChanelIndex == 255)
            orderedChanelIndex = CHANNELS_SIZE - 1;
        else if (orderedChanelIndex >= CHANNELS_SIZE)
            orderedChanelIndex = 0;

        Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));
    } else {
        if (Buttons::get(Button::UP)->pressed) {
            scanning = true;
            forceNext = true;
            direction = ScanDirection::UP;
        } else if (Buttons::get(Button::DOWN)->pressed) {
            scanning = true;
            forceNext = true;
            direction = ScanDirection::DOWN;
        }
    }
}
