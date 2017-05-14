#include "ui.h"
#include "ui_menu.h"


void Ui::MenuHelper::reset() {
    this->activeItems = 0;
    this->selectedItem = 0;
}

void Ui::MenuHelper::addItem(
    const char* text,
    const unsigned char* icon,
    const Ui::MenuHandler handler
) {
    this->menuItems[this->activeItems].text = text;
    this->menuItems[this->activeItems].icon = icon;
    this->menuItems[this->activeItems].handler = handler;

    this->activeItems++;
}

void Ui::MenuHelper::selectNextItem() {
    if (++this->selectedItem >= this->activeItems)
        this->selectedItem = 0;
}

void Ui::MenuHelper::selectPreviousItem() {
    if (--this->selectedItem < 0)
        this->selectedItem = this->activeItems - 1;
}


void Ui::MenuHelper::activateItem() {
    this->menuItems[this->selectedItem].handler();
}


Ui::MenuItem* Ui::MenuHelper::getCurrentItem() {
    return &this->menuItems[this->selectedItem];
}
