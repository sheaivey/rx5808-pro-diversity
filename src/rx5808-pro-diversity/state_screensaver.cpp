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
            pgm_read_byte_near(channelNames + Receiver::activeChannel),
            pgm_read_word_near(channelFreqTable + Receiver::activeChannel),
            nullptr
        );
    }

    void tick() {
        drawScreen.updateScreenSaver(
            Receiver::activeReceiver,
            0,
            Receiver::rssiA,
            Receiver::rssiB
        );

        if (ButtonState::any())
            StateMachine::switchState(StateMachine::lastState);
    }
}
