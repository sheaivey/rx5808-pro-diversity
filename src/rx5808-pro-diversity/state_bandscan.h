#ifndef STATE_BANDSCAN_H
#define STATE_BANDSCAN_H

#include <stdint.h>

#include "channels.h"
#include "state.h"


namespace StateMachine {
    class BandScanStateHandler : public StateMachine::StateHandler {
        private:
            uint8_t orderedChanelIndex = 0;
            uint8_t lastChannelIndex = 0;
            uint8_t rssiData[CHANNELS_SIZE] = { 0 };

        public:
            void onEnter();
            void onExit();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();
    };
}


#endif
