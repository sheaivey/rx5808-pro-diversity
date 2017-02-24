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

#include "timer.h"


void *operator new(size_t size, void *ptr){
  return ptr;
}

#define MAX(a, b) (a > b ? a : b)
#define STATE_BUFFER_SIZE \
    MAX(sizeof(ScreensaverStateHandler), \
    MAX(sizeof(SearchStateHandler), \
    MAX(sizeof(BandScanStateHandler), \
    MAX(sizeof(MenuStateHandler), \
    MAX(sizeof(SettingsStateHandler), \
        sizeof(SettingsRssiStateHandler) \
    )))))
;

namespace StateMachine {
    static char stateBuffer[STATE_BUFFER_SIZE];

    static void onButtonChange();
    static StateHandler *getStateHandler(State stateType);

    State currentState = State::BOOT;
    State lastState = currentState;

    static StateHandler* currentHandler = nullptr;

    void switchState(State newState) {
        if (currentHandler != nullptr) {
            currentHandler->onExit();
            delete currentHandler;
        }

        lastState = currentState;
        currentState = newState;
        currentHandler = getStateHandler(newState);

        if (currentHandler != nullptr) {
            currentHandler->onEnter();
            currentHandler->onInitialDraw();
        }
    }

    void setup() {
        Buttons::registerChangeFunc(onButtonChange);
    }

    static StateHandler *getStateHandler(State state) {
        StateHandler *stateHandler = nullptr;

        switch (state) {
            case State::SEARCH:
                stateHandler = new (&stateBuffer) SearchStateHandler();
                break;

            case State::SCREENSAVER:
                stateHandler = new (&stateBuffer) ScreensaverStateHandler();
                break;

            case State::BANDSCAN:
                stateHandler = new (&stateBuffer) BandScanStateHandler();
                break;

            case State::MENU:
                stateHandler = new (&stateBuffer) MenuStateHandler();
                break;

            case State::SETTINGS:
                stateHandler = new (&stateBuffer) SettingsStateHandler();
                break;

            case State::SETTINGS_RSSI:
                stateHandler = new (&stateBuffer) SettingsRssiStateHandler();
                break;
        }

        return stateHandler;
    }

    void update() {
        if (currentHandler) {
            currentHandler->onUpdate();

            // FIXME: This should probably be handled in the UI module but not
            // 100% on how to decouple them at this stage
            static Timer drawTimer = Timer(OLED_FRAMERATE);
            if (currentHandler
                && Ui::shouldDrawUpdate
                && drawTimer.hasTicked()
            ) {
                currentHandler->onUpdateDraw();
                Ui::shouldDrawUpdate = false;
                drawTimer.reset();
            }
        }
    }

    static void onButtonChange() {
        if (currentHandler != nullptr) {
            currentHandler->onButtonChange();
        }
    }
}
