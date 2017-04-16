#include "ui.h"
#include "ui_state_menu.h"
#include "pstr_helper.h"


using Ui::display;
using Ui::StateMenuHelper;


#define MENU_ITEM_W 16
#define MENU_ITEM_H 16

#define MENU_MARGIN 2
#define MENU_W (MENU_ITEM_W) + (MENU_MARGIN * 2)
#define MENU_TARGET_X (SCREEN_WIDTH - MENU_W - 1)
#define MENU_X (MENU_TARGET_X + this->slideX)
#define MENU_H (SCREEN_HEIGHT)


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

        if (this->visible) {
            this->slideX = MENU_W;
        }

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

    if (MENU_X != MENU_TARGET_X) {
        this->slideX -= 8;
        if (this->slideX < 0)
            this->slideX = 0;
    }

    display.fillRect(
        MENU_X,
        0,
        MENU_W,
        MENU_H,
        BLACK
    );

    display.drawFastVLine(
        MENU_X - 1,
        0,
        MENU_H,
        WHITE
    );

    display.setTextSize(1);

    for (uint8_t i = 0; i < this->activeItems; i++) {
        if (this->selectedItem == i) {
            display.fillRect(
                MENU_X,
                MENU_ITEM_H * i,
                MENU_ITEM_W,
                MENU_ITEM_H,
                WHITE
            );

            display.setTextColor(BLACK);
        } else {
            display.setTextColor(WHITE);
        }

        display.setCursor(
            MENU_X + MENU_MARGIN,
            MENU_ITEM_H * i
        );

        display.print(this->menuItems[i].text(this->state));
    }
}
