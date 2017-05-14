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
    enum class PressType : uint8_t {
        SHORT,
        LONG,
        HOLDING
    };


    struct ButtonState {
        unsigned long lastDebounceTime = 0;
        bool lastReading = false;

        bool pressed = false;
        unsigned long changeTime = 0;
    };


    typedef void(*ChangeFunc)(Button button, PressType pressType);


    extern uint32_t lastChangeTime;

    void update();

    const ButtonState *get(Button button);
    const bool any();

    void registerChangeFunc(ChangeFunc func);
    void deregisterChangeFunc(ChangeFunc func);
}


#endif
