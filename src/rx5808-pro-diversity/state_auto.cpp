#include <Arduino.h>

#include "state_auto.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
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


static void onButtonChange();


void StateMachine::AutoStateHandler::onEnter() {
    ButtonState::registerChangeFunc(onButtonChange);
    //drawScreen.seekMode(STATE_SEEK);
}

void StateMachine::AutoStateHandler::onExit() {
    ButtonState::deregisterChangeFunc(onButtonChange);
}

void StateMachine::AutoStateHandler::onTick() {
    /*drawScreen.updateSeekMode(
        STATE_SEEK,
        Receiver::activeChannel,
        Channels::getOrderedIndex(Receiver::activeChannel),
        Receiver::rssiA,
        Channels::getFrequency(Receiver::activeChannel),
        RSSI_SEEK_TRESHOLD,
        !scanning
    );*/

    if (scanning) {
        if (!forceNext && Receiver::rssiA >= RSSI_SEEK_TRESHOLD)
            scanning = false;

        int nextChannel = Receiver::activeChannel
            + static_cast<int8_t>(direction);
        if (nextChannel < 0)
            nextChannel = CHANNEL_MAX_INDEX;
        if (nextChannel > CHANNEL_MAX_INDEX)
            nextChannel = 0;

        Receiver::setChannel(nextChannel);

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
