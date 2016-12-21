#ifndef BUTTONS_H
#define BUTTONS_H

struct ButtonState {
    bool up = false;
    bool down = false;
    bool mode = false;
    bool save = false;
};

void updateButtons();
extern struct ButtonState ButtonState;

#endif
