#include <Arduino.h>
#include <avr/pgmspace.h>

#include "state_manual.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "screens.h"
#include "buttons.h"


extern screens drawScreen;


static void onButtonChange();


void StateMachine::ManualStateHandler::onEnter() {
    drawScreen.seekMode(STATE_MANUAL);

    ButtonState::registerChangeFunc(onButtonChange);
}

void StateMachine::ManualStateHandler::onExit() {
    ButtonState::deregisterChangeFunc(onButtonChange);
}

void StateMachine::ManualStateHandler::onTick() {
    drawScreen.updateSeekMode(
        STATE_MANUAL,
        Receiver::activeChannel,
        Channels::getOrderedIndex(Receiver::activeChannel),
        Receiver::rssiA,
        Channels::getFrequency(Receiver::activeChannel),
        RSSI_SEEK_TRESHOLD,
        true
    );

    if ((millis() - ButtonState::lastPressTime) > 1000)
        onButtonChange();
}


static void onButtonChange() {
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
