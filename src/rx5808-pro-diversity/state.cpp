#include <stddef.h>

#include "state.h"

#include "state_screensaver.h"
#include "state_search.h"
#include "state_bandscan.h"
#include "state_menu.h"
#include "state_settings.h"
#include "state_settings_rssi.h"

#include "ui.h"
#include "buttons.h"


namespace StateMachine {
    static void onButtonChange();

    static ScreensaverStateHandler screensaverHandler;
    static SearchStateHandler searchHandler;
    static BandScanStateHandler bandHandler;
    static MenuStateHandler menuHandler;
    static SettingsStateHandler settingsHandler;
    static SettingsRssiStateHandler settingsRssiHandler;

    static StateHandler* handlers[STATE_COUNT] = {
        nullptr,
        &searchHandler,
        &bandHandler,
        &screensaverHandler,
        &menuHandler,
        &settingsHandler,
        &settingsRssiHandler
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

        if (lastHandler != nullptr) {
            lastHandler->onExit();
        }

        lastState = currentState;
        currentState = newState;
        currentHandler = newHandler;

        if (newHandler != nullptr) {
            newHandler->onEnter();
            newHandler->onInitialDraw();
        }
    }

    void setup() {
        Buttons::registerChangeFunc(onButtonChange);
    }

    void update() {
        if (currentHandler) {
            currentHandler->onUpdate();

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

    static void onButtonChange() {
        if (currentHandler != nullptr) {
            currentHandler->onButtonChange();
        }
    }
}
