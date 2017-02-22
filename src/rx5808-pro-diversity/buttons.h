#ifndef BUTTONS_H
#define BUTTONS_H


#include <stdint.h>


#define BUTTON_HOOKS_MAX 4


enum class Button : uint8_t {
    UP,
    DOWN,
    MODE,
    SAVE,
    COUNT
};
#define BUTTON_COUNT static_cast<uint8_t>(Button::COUNT)


namespace Buttons {
    typedef void(*ChangeFunc)();


    struct ButtonState {
        unsigned long lastDebounceTime = 0;
        bool lastReading = false;

        bool pressed = false;
        unsigned long pressTime = 0;
    };


    extern uint32_t lastPressTime;


    void update();

    const ButtonState get(Button button);
    const bool any();
    unsigned long waitForRelease(Button button);

    void registerChangeFunc(ChangeFunc func);
    void deregisterChangeFunc(ChangeFunc func);
}


#endif
