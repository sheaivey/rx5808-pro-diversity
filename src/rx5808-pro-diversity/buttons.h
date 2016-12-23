#ifndef BUTTONS_H
#define BUTTONS_H


#include <stdint.h>


enum class Button : uint8_t {
    UP,
    DOWN,
    MODE,
    SAVE,
    COUNT
};
#define BUTTON_COUNT static_cast<uint8_t>(Button::COUNT)


namespace ButtonState {
    extern uint32_t lastPressTime;


    void update();

    const bool get(Button button);
    const bool any();
    unsigned long waitForRelease(Button button);
}


#endif
