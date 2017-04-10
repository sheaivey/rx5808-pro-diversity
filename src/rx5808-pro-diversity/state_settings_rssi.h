#ifndef STATE_SETTINGS_RSSI_H
#define STATE_SETTINGS_RSSI_H


#include "state.h"


namespace StateMachine {
    class SettingsRssiStateHandler : public StateMachine::StateHandler {
        private:
            enum class InternalState : uint8_t {
                WAIT_FOR_LOW,
                SCANNING_LOW,
                WAIT_FOR_HIGH,
                SCANNING_HIGH,
                DONE
            };


            InternalState internalState = InternalState::WAIT_FOR_LOW;
            uint8_t currentSweep = 0;


        public:
            void onEnter();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange(Button button, Buttons::PressType pressType);
    };
}


#endif
