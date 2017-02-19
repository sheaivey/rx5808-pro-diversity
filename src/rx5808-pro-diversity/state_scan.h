#ifndef STATE_SCAN_H
#define STATE_SCAN_H


#include "state.h"


namespace StateMachine {
    class ScanStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onExit();
            void onTick();

            void onInitialDraw();
            void onUpdateDraw();
    };
}


#endif
