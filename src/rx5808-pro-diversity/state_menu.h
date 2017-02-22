#ifndef STATE_MENU_H
#define STATE_MENU_H


#include "state.h"


namespace StateMachine {
    class MenuStateHandler : public StateMachine::StateHandler {
        public:
            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange();
    };
}


#endif
