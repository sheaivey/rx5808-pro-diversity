#ifndef STATE_SEARCH_H
#define STATE_SEARCH_H


#include "state.h"


namespace StateMachine {
    class SearchStateHandler : public StateMachine::StateHandler {
        public:
            void onTick();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange();
    };
}


#endif
