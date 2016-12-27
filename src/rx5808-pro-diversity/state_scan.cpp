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
        drawScreen.updateBandScanMode(
            false,
            orderedChanelIndex,
            Receiver::rssiA,
            Channels::getName(Receiver::activeChannel),
            Channels::getFrequency(Receiver::activeChannel),
            EepromSettings.rssiAMin,
            EepromSettings.rssiAMax);

        orderedChanelIndex = (orderedChanelIndex + 1) % CHANNEL_MAX_INDEX;
        uint8_t realChannelIndex = Channels::getOrderedIndex(
            orderedChanelIndex);

        Receiver::setChannel(realChannelIndex);
    }
}
