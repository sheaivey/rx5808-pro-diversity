#include <Arduino.h>
#include <stdint.h>

#include "buttons.h"
#include "settings.h"


struct Buttons::ButtonState states[BUTTON_COUNT];
static Buttons::ChangeFunc changeFuncs[BUTTON_HOOKS_MAX] = { nullptr };


namespace Buttons {
    static void runChangeFuncs(Button button, PressType pressType);
    static void updateButton(
        const Button button,
        struct Buttons::ButtonState &state,
        const uint8_t pin
    );


    uint32_t lastChangeTime = 0;


    void update() {
        #define UPDATE_BUTTON(button) \
            updateButton( \
                Button::button, \
                states[static_cast<size_t>(Button::button)], \
                PIN_BUTTON_ ## button \
            );

        UPDATE_BUTTON(UP);
        UPDATE_BUTTON(DOWN);
        UPDATE_BUTTON(MODE);
        UPDATE_BUTTON(SAVE);

        #undef UPDATE_BUTTON
    }

    const ButtonState *get(Button button) {
        return &states[static_cast<size_t>(button)];
    }

    const bool any() {
        for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
            if (states[i].pressed)
                return true;
        }

        return false;
    }

    void registerChangeFunc(ChangeFunc func) {
        for (uint8_t i = 0; i < BUTTON_HOOKS_MAX; i++) {
            if (changeFuncs[i] == nullptr) {
                changeFuncs[i] = func;
                return;
            }
        }
    }

    void deregisterChangeFunc(ChangeFunc func) {
        for (uint8_t i = 0; i < BUTTON_HOOKS_MAX; i++) {
            if (changeFuncs[i] == func) {
                changeFuncs[i] = nullptr;
                return;
            }
        }
    }

    static void runChangeFuncs(Button button, PressType pressType) {
        for (uint8_t i = 0; i < BUTTON_HOOKS_MAX; i++) {
            if (changeFuncs[i] != nullptr) {
                changeFuncs[i](button, pressType);
            }
        }
    }

    static void updateButton(
        const Button button,
        struct ButtonState &state,
        const uint8_t pin
    ) {
        const uint8_t reading = !digitalRead(pin); // Invert as we use pull-ups.

        if (reading != state.lastReading) {
            state.lastDebounceTime = millis();
        }

        state.lastReading = reading;

        if (
            reading != state.pressed &&
            (millis() - state.lastDebounceTime) >= BUTTON_DEBOUNCE_DELAY
        ) {
            state.pressed = reading;

            uint32_t prevChangeTime = state.changeTime;
            state.changeTime = millis();
            lastChangeTime = state.changeTime;

            if (!state.pressed) {
                uint32_t duration = state.changeTime - prevChangeTime;

                if (duration < 500)
                    runChangeFuncs(button, PressType::SHORT);
                else if (duration < 2000)
                    runChangeFuncs(button, PressType::LONG);
            }
        }

        if (state.pressed) {
            uint32_t duration = millis() - state.changeTime;

            if (duration >= 2000)
                runChangeFuncs(button, PressType::HOLDING);
        }
    }
}
