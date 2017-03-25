#include <Arduino.h>
#include <stdint.h>

#include "buttons.h"
#include "settings.h"


static const bool updateButton(
    const uint8_t pin,
    struct Buttons::ButtonState &state
);


struct Buttons::ButtonState states[BUTTON_COUNT];

static bool needCallChangeFuncs = false;
static Buttons::ChangeFunc changeFuncs[BUTTON_HOOKS_MAX] = { nullptr };


namespace Buttons {
    static void runChangeFuncs();
    uint32_t lastPressTime = 0;


    void update() {
        #define UPDATE_BUTTON(button) \
            updateButton( \
                PIN_BUTTON_ ## button, \
                states[static_cast<size_t>(Button::button)] \
            );

        UPDATE_BUTTON(UP);
        UPDATE_BUTTON(DOWN);
        UPDATE_BUTTON(MODE);
        UPDATE_BUTTON(SAVE);

        // HACK: this is super gross, better way of doing it?
        if (needCallChangeFuncs) {
            runChangeFuncs();
            needCallChangeFuncs = false;
            lastPressTime = millis();
        }

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

    unsigned long waitForRelease(Button button) {
        const unsigned long startTime = millis();

        while (get(button)->pressed) {
            update();
            delay(BUTTON_DEBOUNCE_DELAY);
        }

        return millis() - startTime;
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

    static void runChangeFuncs() {
        for (uint8_t i = 0; i < BUTTON_HOOKS_MAX; i++) {
            if (changeFuncs[i] != nullptr) {
                changeFuncs[i]();
            }
        }
    }
}


static const bool updateButton(
    const uint8_t pin,
    struct Buttons::ButtonState &state
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
        state.pressTime = millis();
        needCallChangeFuncs = true;
    }
}