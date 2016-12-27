#include <avr/pgmspace.h>

#include "state_scan.h"

#include "receiver.h"
#include "channels.h"
#include "screens.h"
#include "buttons.h"
#include "state.h"


extern screens drawScreen;


namespace StateScreensaver {
    void enter() {
        drawScreen.screenSaver(
            0,
            Channels::getName(Receiver::activeChannel),
            Channels::getFrequency(Receiver::activeChannel),
            nullptr
        );
    }

    void tick() {
        drawScreen.updateScreenSaver(
            Receiver::activeReceiver,
            Receiver::rssiA,
            Receiver::rssiA,
            Receiver::rssiB
        );

        if (ButtonState::any())
            StateMachine::switchState(StateMachine::lastState);
    }
}
