#ifndef STATE_SETTINGS_RSSI_H
#define STATE_SETTINGS_RSSI_H


#include "state.h"


namespace StateMachine {
    class SettingsRssiStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange();
    };
}


#endif
