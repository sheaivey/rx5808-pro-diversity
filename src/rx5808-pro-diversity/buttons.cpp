#include <Arduino.h>
#include <stdint.h>

#include "buttons.h"
#include "settings/settings.h"


struct ButtonHistory {
    unsigned long lastDebounceTime = 0;
    bool lastReading = false;
};


static const bool updateButton(
    const int pin,
    bool &state,
    struct ButtonHistory &history
);


static struct ButtonHistory histories[static_cast<size_t>(Button::COUNT)];
static bool states[static_cast<size_t>(Button::COUNT)];


namespace ButtonState {
    void update() {
        #define UPDATE_BUTTON(button) \
            states[static_cast<size_t>(Button::button)] = updateButton( \
                PIN_BUTTON_ ## button, \
                states[static_cast<size_t>(Button::button)], \
                histories[static_cast<size_t>(Button::button)] \
            );

        UPDATE_BUTTON(UP);
        UPDATE_BUTTON(DOWN);
        UPDATE_BUTTON(MODE);
        UPDATE_BUTTON(SAVE);

        #undef UPDATE_BUTTON
    }

    const bool get(Button button) {
        return states[static_cast<size_t>(button)];
    }

    unsigned long waitForRelease(Button button) {
        const unsigned long startTime = millis();

        while (get(button)) {
            update();
            delay(BUTTON_DEBOUNCE_DELAY);
        }

        return millis() - startTime;
    }
}


static const bool updateButton(
    const int pin,
    bool &state,
    struct ButtonHistory &history
) {
    const int reading = !digitalRead(pin); // Invert as we use pull-ups.

    if (reading != history.lastReading) {
        history.lastDebounceTime = millis();
    }

    history.lastReading = reading;

    if ((millis() - history.lastDebounceTime) >= BUTTON_DEBOUNCE_DELAY) {
        if (reading != state) {
            return reading;
        }
    }

    return state;
}
