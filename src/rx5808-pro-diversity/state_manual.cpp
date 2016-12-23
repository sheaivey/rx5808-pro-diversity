#include <avr/pgmspace.h>

#include "state_manual.h"

#include "settings.h"
#include "internal_settings.h"
#include "eeprom_settings.h"
#include "receiver.h"
#include "channels.h"
#include "screens.h"
#include "buttons.h"


extern screens drawScreen;


namespace StateManual {
    void enter() {
        drawScreen.seekMode(STATE_MANUAL);
    }

    void tick() {
        drawScreen.updateSeekMode(
            STATE_MANUAL,
            Receiver::activeChannel,
            Receiver::activeChannel,
            Receiver::rssiA,
            pgm_read_word_near(channelFreqTable + Receiver::activeChannel),
            RSSI_SEEK_TRESHOLD,
            true
        );

        if (ButtonState::get(Button::UP)) {
            Receiver::setChannel(
                (Receiver::activeChannel + 1) % CHANNEL_MAX_INDEX
            );
        } else if (ButtonState::get(Button::DOWN)) {
            Receiver::setChannel(
                (Receiver::activeChannel - 1) % CHANNEL_MAX_INDEX
            );
        }
    }
}
