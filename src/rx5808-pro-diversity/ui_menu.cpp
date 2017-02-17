#include "ui.h"
#include "ui_menu.h"


namespace Ui {
    Ui::MenuComponent menu;
}


#define MENU_ITEMS_MAX 8


struct MenuItem {
    const char* text = nullptr;
    Ui::MenuHandler handler = nullptr;
};


static MenuItem menuItems[MENU_ITEMS_MAX];
static int activeItems = 0;
static int selectedItem = 2;


void Ui::MenuComponent::drawInitial() {
    for (int i = 0; i < activeItems; i++) {
        const MenuItem* item = &menuItems[i];

        if (i == selectedItem) {
            Ui::display.fillRect(0, 11 + i * (8 + 2), SCREEN_WIDTH, 9, WHITE);
            Ui::display.fillTriangle(
                0, 11 + i * (8 + 2) + 0,
                8, 11 + i * (8 + 2) + 0,
                0, 11 + i * (8 + 2) + 8,
                BLACK
            );

            Ui::display.setTextColor(BLACK);
            Ui::display.setCursor(10, 11 + i * (8 + 2) + 1);
        } else {
            Ui::display.setCursor(0, 11 + i * (8 + 2) + 1);
            Ui::display.setTextColor(WHITE);
        }

        Ui::display.print(item->text);
    }
}

void Ui::MenuComponent::drawUpdate() {
    Ui::display.fillRect(0, 11, SCREEN_WIDTH, 10 * activeItems, 0);
    this->drawInitial();
}


void Ui::MenuComponent::reset() {
    activeItems = 0;
    selectedItem = 0;
}

void Ui::MenuComponent::addItem(const char* text, Ui::MenuHandler handler) {
    menuItems[activeItems].text = text;
    menuItems[activeItems].handler = handler;

    activeItems++;
}

void Ui::MenuComponent::selectNextItem() {
    if (++selectedItem >= activeItems)
        selectedItem = 0;

    Ui::MenuComponent::drawInitial();
}

void Ui::MenuComponent::selectPreviousItem() {
    if (--selectedItem < 0)
        selectedItem = activeItems - 1;

    Ui::MenuComponent::drawInitial();
}


void Ui::MenuComponent::activateItem() {
    menuItems[selectedItem].handler();
}
