#include <avr/pgmspace.h>

#include "state_scan.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "screens.h"


extern screens drawScreen;

static uint8_t orderedChanelIndex = 0;
static uint8_t lastChannelIndex = 0;


namespace StateScan {
    void enter() {
        orderedChanelIndex = 0;
        lastChannelIndex = Receiver::activeChannel;
        drawScreen.bandScanMode(STATE_SCAN);
    }

    void exit() {
        Receiver::setChannel(lastChannelIndex);
    }

    void tick() {
        uint8_t channelName = pgm_read_byte(
            channelNames + Receiver::activeChannel
        );

        uint16_t channelFreq = pgm_read_byte(
            channelFreqTable + Receiver::activeChannel
        );

        drawScreen.updateBandScanMode(
            false,
            orderedChanelIndex,
            Receiver::rssiA,
            channelName,
            channelFreq,
            EepromSettings.rssiAMin,
            EepromSettings.rssiAMax);

        orderedChanelIndex = (orderedChanelIndex + 1) % CHANNEL_MAX_INDEX;
        uint8_t realChannelIndex = pgm_read_byte(
            channelFreqOrderedIndex + orderedChanelIndex
        );

        Receiver::setChannel(realChannelIndex);
    }
}
