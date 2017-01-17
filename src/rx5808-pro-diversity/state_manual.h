#ifndef STATE_MANUAL_H
#define STATE_MANUAL_H


#include "state.h"


namespace StateMachine {
    class ManualStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onExit();
            void onTick();
    };
}


#endif
