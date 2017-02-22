#ifndef STATE_BANDSCAN_H
#define STATE_BANDSCAN_H


#include "state.h"


namespace StateMachine {
    class BandScanStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onExit();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange();
    };
}


#endif
