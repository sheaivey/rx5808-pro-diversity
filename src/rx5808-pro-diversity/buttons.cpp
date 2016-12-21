#include <Arduino.h>
#include <stdint.h>

#include "buttons.h"
#include "settings.h"

struct ButtonHistory {
    unsigned long lastDebounceTime = 0;
    bool lastReading = false;
};

struct ButtonHistories {
    struct ButtonHistory up;
    struct ButtonHistory down;
    struct ButtonHistory mode;
    struct ButtonHistory save;
};

static bool readButton(
    const int pin,
    bool &debouncedState,
    struct ButtonHistory &history
);

static struct ButtonHistories histories;
struct ButtonState ButtonState;

void updateButtons() {
    ButtonState.up = readButton(
        PIN_BUTTON_UP,
        ButtonState.up,
        histories.up
    );

    ButtonState.down = readButton(
        PIN_BUTTON_DOWN,
        ButtonState.down,
        histories.down
    );

    ButtonState.mode = readButton(
        PIN_BUTTON_MODE,
        ButtonState.mode,
        histories.mode
    );

    ButtonState.save = readButton(
        PIN_BUTTON_SAVE,
        ButtonState.save,
        histories.save
    );
}

static bool readButton(
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
