#include <stddef.h>

#include "state.h"

#include "state_screensaver.h"
#include "state_scan.h"
#include "state_auto.h"
#include "state_menu.h"

#include "ui.h"


namespace StateMachine {
    static ScreensaverStateHandler screensaverHandler;
    static ScanStateHandler scanHandler;
    static AutoStateHandler autoHandler;
    static MenuStateHandler menuHandler;

    static StateHandler* handlers[STATE_COUNT] = {
        nullptr,
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

        if (newHandler != nullptr) {
            newHandler->onEnter();
            newHandler->onInitialDraw();
        }
    }

    void tick() {
        if (currentHandler) {
            currentHandler->onTick();

            // FIXME: This should probably be handled in the UI module but not
            // 100% on how to decouple them at this stage
            static long lastDraw = 0;
            if (currentHandler
                && Ui::shouldDrawUpdate
                && millis() > lastDraw + OLED_FRAMERATE
            ) {
                currentHandler->onUpdateDraw();
                lastDraw = millis();
                Ui::shouldDrawUpdate = false;
            }
        }
    }
}
