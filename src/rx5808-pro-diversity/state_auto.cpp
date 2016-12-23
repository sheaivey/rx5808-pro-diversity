#include <Arduino.h>
#include <avr/pgmspace.h>

#include "state_manual.h"

#include "settings.h"
#include "internal_settings.h"
#include "eeprom_settings.h"
#include "receiver.h"
#include "channels.h"
#include "screens.h"
#include "buttons.h"


enum class ScanDirection : int8_t {
    UP = 1,
    DOWN = -1
};


static bool scanning = true;
static ScanDirection direction = ScanDirection::UP;
static bool forceNext = false;

extern screens drawScreen;


namespace StateAuto {
    static void onButtonChange();


    void enter() {
        ButtonState::registerChangeFunc(onButtonChange);
        drawScreen.seekMode(STATE_SEEK);
    }

    void exit() {
        ButtonState::deregisterChangeFunc(onButtonChange);
    }

    void tick() {
        drawScreen.updateSeekMode(
            STATE_SEEK,
            Receiver::activeChannel,
            pgm_read_word_near(
                channelFreqOrderedIndex + Receiver::activeChannel
            ),
            Receiver::rssiA,
            pgm_read_word_near(channelFreqTable + Receiver::activeChannel),
            RSSI_SEEK_TRESHOLD,
            !scanning
        );

        if (scanning) {
            if (!forceNext && Receiver::rssiA >= RSSI_SEEK_TRESHOLD)
                scanning = false;

            Receiver::setChannel(
                (Receiver::activeChannel + static_cast<int8_t>(direction))
                    % CHANNEL_MAX_INDEX
            );

            if (forceNext)
                forceNext = false;
        }
    }

    static void onButtonChange() {
        if (ButtonState::get(Button::UP)) {
            scanning = true;
            forceNext = true;
            direction = ScanDirection::UP;
        } else if (ButtonState::get(Button::DOWN)) {
            scanning = true;
            forceNext = true;
            direction = ScanDirection::DOWN;
        }
    }
}
