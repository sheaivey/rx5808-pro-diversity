#ifndef BUTTONS_H
#define BUTTONS_H

#define BUTTON_COUNT 4

enum Button {
    UP,
    DOWN,
    MODE,
    SAVE
};

void updateButtons();
unsigned long waitForButtonRelease(Button button);

extern bool ButtonState[BUTTON_COUNT];

#endif
