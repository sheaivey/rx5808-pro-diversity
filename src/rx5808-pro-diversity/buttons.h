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


namespace ButtonState {
    void update();

    const bool get(Button button);
    unsigned long waitForRelease(Button button);
}


#endif
