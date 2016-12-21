#include <Arduino.h>
#include <stdint.h>

#include "buttons.h"
#include "settings.h"


struct ButtonHistory {
    unsigned long lastDebounceTime = 0;
    bool lastReading = false;
};


static bool updateButton(
    const int pin,
    bool &debouncedState,
    struct ButtonHistory &history
);


static struct ButtonHistory histories[BUTTON_COUNT];
bool ButtonState[BUTTON_COUNT];


void updateButtons() {
    #define UPDATE_BUTTON(button) \
        ButtonState[Button::button] = updateButton( \
            PIN_BUTTON_ ## button, \
            ButtonState[Button::button], \
            histories[Button::button] \
        );

    UPDATE_BUTTON(UP);
    UPDATE_BUTTON(DOWN);
    UPDATE_BUTTON(MODE);
    UPDATE_BUTTON(SAVE);

    #undef UPDATE_BUTTON
}

unsigned long waitForButtonRelease(Button button) {
    const unsigned long startTime = millis();

    while (ButtonState[button])
        updateButtons();

    return millis() - startTime;
}


static bool updateButton(
    const int pin,
    bool &debouncedState,
    struct ButtonHistory &history
) {
    const int reading = !digitalRead(pin); // Invert as we use pull-ups.

    if (reading != history.lastReading) {
        history.lastDebounceTime = millis();
    }

    history.lastReading = reading;

    if ((millis() - history.lastDebounceTime) >= BUTTON_DEBOUNCE_DELAY) {
        if (reading != debouncedState) {
            return reading;
        }
    }

    return debouncedState;
}

