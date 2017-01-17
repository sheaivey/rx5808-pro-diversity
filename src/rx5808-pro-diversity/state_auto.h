#ifndef STATE_AUTO_H
#define STATE_AUTO_H


#include "state.h"


namespace StateMachine {
    class AutoStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onExit();
            void onTick();
    };
}


#endif
