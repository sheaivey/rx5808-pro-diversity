#include <stddef.h>

#include "state.h"

#include "state_screensaver.h"
#include "state_scan.h"
#include "state_manual.h"
#include "state_auto.h"
#include "state_menu.h"


namespace StateMachine {
    static ScreensaverStateHandler screensaverHandler;
    static ManualStateHandler manualHandler;
    static ScanStateHandler scanHandler;
    static AutoStateHandler autoHandler;
    static MenuStateHandler menuHandler;

    static StateHandler* handlers[STATE_COUNT] = {
        nullptr,
        &manualHandler,
        &scanHandler,
        &autoHandler,
        &screensaverHandler,
        &menuHandler
    };

    State currentState = State::BOOT;
    State lastState = currentState;

    static StateHandler* currentHandler =
        handlers[static_cast<size_t>(currentState)];


    void switchState(State newState) {
        StateHandler* lastHandler =
            handlers[static_cast<size_t>(currentState)];
        StateHandler* newHandler =
            handlers[static_cast<size_t>(newState)];

        if (lastHandler != nullptr)
            lastHandler->onExit();

        lastState = currentState;
        currentState = newState;
        currentHandler = newHandler;

        if (newHandler != nullptr)
            newHandler->onEnter();
    }

    void tick() {
        if (currentHandler)
            currentHandler->onTick();
    }
}
