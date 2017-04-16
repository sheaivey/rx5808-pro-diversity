#include "ui.h"
#include "ui_state_menu.h"
#include "pstr_helper.h"


using Ui::display;
using Ui::StateMenuHelper;


void StateMenuHelper::addItem(
    const MenuText text,
    const MenuHandler handler
) {
    this->menuItems[this->activeItems].text = text;
    this->menuItems[this->activeItems].handler = handler;

    this->activeItems++;
}

bool StateMenuHelper::handleButtons(
    Button button,
    Buttons::PressType pressType
) {
    if (button == Button::MODE && pressType == Buttons::PressType::LONG) {
        this->visible = !this->visible;
        if (!this->visible)
            Ui::needFullRedraw();

        return true;
    }

    if (!this->isVisible())
        return false;

    switch (button) {
        case Button::UP:
            if (--this->selectedItem < 0)
                this->selectedItem = this->activeItems - 1;
            break;

        case Button::DOWN:
            if (++this->selectedItem >= this->activeItems)
                this->selectedItem = 0;
            break;

        case Button::MODE:
            this->menuItems[this->selectedItem].handler(this->state);
            break;
    }

    return true;
}

void StateMenuHelper::draw() {
    if (!this->isVisible())
        return;

    #define MENU_ITEM_W 24

    #define MENU_W (MENU_ITEM_W * this->activeItems)
    #define MENU_X_L (SCREEN_WIDTH_MID - (MENU_W / 2))
    #define MENU_X_R (SCREEN_WIDTH_MID + (MENU_W / 2))
    #define MENU_H ((CHAR_HEIGHT * 2) + 8)
    #define MENU_Y (SCREEN_HEIGHT - MENU_H)


    #define MENU_ITEM_X_START (MENU_X_L)
    #define MENU_ITEM_X_END (MENU_X_R)
    #define MENU_ITEM_X_OFFSET ((MENU_ITEM_W / 2) - (CHAR_WIDTH * 2 / 2))
    #define MENU_ITEM_Y (MENU_Y + 4)

    display.fillRect(
        MENU_X_L,
        MENU_Y,
        MENU_W,
        MENU_H * 2,
        BLACK
    );

    display.drawRect(
        MENU_X_L,
        MENU_Y,
        MENU_W,
        MENU_H * 2,
        WHITE
    );

    display.setTextSize(2);

    for (uint8_t i = 0; i < this->activeItems; i++) {
        if (this->selectedItem == i) {
            display.fillRect(
                MENU_ITEM_X_START + 1 + (MENU_ITEM_W * i),
                MENU_Y,
                MENU_ITEM_W,
                MENU_H,
                WHITE
            );

            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }

        display.setCursor(
            MENU_ITEM_X_START + MENU_ITEM_X_OFFSET + (MENU_ITEM_W * i),
            MENU_ITEM_Y
        );

        display.print(this->menuItems[i].text(this->state));
    }
}
