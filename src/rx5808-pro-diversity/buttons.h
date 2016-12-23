#ifndef BUTTONS_H
#define BUTTONS_H


#include <stdint.h>


#define BUTTON_HOOKS_MAX 8


enum class Button : uint8_t {
    UP,
    DOWN,
    MODE,
    SAVE,
    COUNT
};
#define BUTTON_COUNT static_cast<uint8_t>(Button::COUNT)


namespace ButtonState {
    typedef void(*ChangeFunc)();


    extern uint32_t lastPressTime;


    void update();

    const bool get(Button button);
    const bool any();
    unsigned long waitForRelease(Button button);

    void registerChangeFunc(ChangeFunc func);
    void deregisterChangeFunc(ChangeFunc func);
}


#endif
