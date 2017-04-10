#include "ui.h"
#include "ui_menu.h"


namespace Ui {
    Ui::MenuHelper menu;
}


#define MENU_ITEMS_MAX 8


Ui::MenuItem menuItems[MENU_ITEMS_MAX];
static int activeItems = 0;
static int selectedItem = 0;


void Ui::MenuHelper::reset() {
    activeItems = 0;
    selectedItem = 0;
}

void Ui::MenuHelper::addItem(
    const char* text,
    const unsigned char* icon,
    const Ui::MenuHandler handler
) {
    menuItems[activeItems].text = text;
    menuItems[activeItems].icon = icon;
    menuItems[activeItems].handler = handler;

    activeItems++;
}

void Ui::MenuHelper::selectNextItem() {
    if (++selectedItem >= activeItems)
        selectedItem = 0;
}

void Ui::MenuHelper::selectPreviousItem() {
    if (--selectedItem < 0)
        selectedItem = activeItems - 1;
}


void Ui::MenuHelper::activateItem() {
    menuItems[selectedItem].handler();
}


Ui::MenuItem* Ui::MenuHelper::getCurrentItem() {
    return &menuItems[selectedItem];
}
