#include <stddef.h>

#include "state.h"

#include "state_screensaver.h"
#include "state_scan.h"
#include "state_manual.h"
#include "state_auto.h"


namespace StateMachine {
    static ScreensaverStateHandler screensaverHandler;
    static ManualStateHandler manualHandler;
    static ScanStateHandler scanHandler;
    static AutoStateHandler autoHandler;

    static StateHandler* handlers[STATE_COUNT] = {
        nullptr,
        &manualHandler,
        &scanHandler,
        &autoHandler,
        &screensaverHandler
    };

    State currentState = State::SCREENSAVER;
    State lastState = currentState;

    static StateHandler* currentHandler =
        handlers[static_cast<size_t>(currentState)];


    void switchState(State newState) {
        StateHandler* lastHandler =
            handlers[static_cast<size_t>(currentState)];
        StateHandler* newHandler =
            handlers[static_cast<size_t>(newState)];

        if (lastHandler)
            lastHandler->onExit();

        lastState = currentState;
        currentState = newState;
        currentHandler = newHandler;

        if (newHandler)
            lastHandler->onEnter();
    }

    void tick() {
        if (currentHandler)
            currentHandler->onTick();
    }
}
