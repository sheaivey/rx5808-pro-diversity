#ifndef STATE_SCREENSAVER_H
#define STATE_SCREENSAVER_H


#include "state.h"


namespace StateMachine {
    class ScreensaverStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onExit();
            void onTick();

            void onInitialDraw();
            void onUpdateDraw();
    };
}


#endif
