#include <avr/pgmspace.h>

#include "state_screensaver.h"

#include "receiver.h"
#include "channels.h"
#include "screens.h"
#include "buttons.h"
#include "state.h"


extern screens drawScreen;


void StateMachine::ScreensaverStateHandler::onEnter() {
    drawScreen.screenSaver(
            0,
            Channels::getName(Receiver::activeChannel),
            Channels::getFrequency(Receiver::activeChannel),
            nullptr
        );
}

void StateMachine::ScreensaverStateHandler::onTick() {
    drawScreen.updateScreenSaver(
            Receiver::activeReceiver,
            Receiver::rssiA,
            Receiver::rssiA,
            Receiver::rssiB
        );

        if (ButtonState::any())
            StateMachine::switchState(StateMachine::lastState);
}
