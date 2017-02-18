#include "ui.h"
#include "ui_menu.h"


namespace Ui {
    Ui::MenuComponent menu;
}


#define MENU_ITEMS_MAX 8


struct MenuItem {
    const char* text = nullptr;
    Ui::MenuHandler handler = nullptr;
    const unsigned char* icon = nullptr;
};


static MenuItem menuItems[MENU_ITEMS_MAX];
static int activeItems = 0;
static int selectedItem = 0;


void Ui::MenuComponent::drawInitial() {
    const MenuItem* item = &menuItems[selectedItem];
    const uint8_t charLen = strlen(item->text);

    Ui::display.setTextSize(2);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(
        SCREEN_WIDTH / 2 - (charLen * ((CHAR_WIDTH + 1) * 2)) / 2,
        SCREEN_HEIGHT - (CHAR_HEIGHT + 2) * 2
    );
    Ui::display.print(item->text);

    if (item->icon) {
        Ui::clearRect(
            SCREEN_WIDTH / 2 - 32 / 2,
            7,
            32,
            32
        );

        Ui::display.drawBitmap(
            SCREEN_WIDTH / 2 - 32 / 2,
            7,
            item->icon,
            32,
            32,
            WHITE
        );
    }

    #define TRIANGLE_SIZE 4
    #define TRIANGLE_MARGIN 2

    Ui::display.fillTriangle(
        SCREEN_WIDTH - 1 - (TRIANGLE_SIZE),
        SCREEN_HEIGHT / 2 + TRIANGLE_MARGIN,

        SCREEN_WIDTH - 1 - (TRIANGLE_SIZE / 2),
        SCREEN_HEIGHT / 2 + TRIANGLE_MARGIN + (TRIANGLE_SIZE),

        SCREEN_WIDTH - 1,
        SCREEN_HEIGHT / 2 + TRIANGLE_MARGIN,

        WHITE
    );

    Ui::display.fillTriangle(
        SCREEN_WIDTH - 1 - (TRIANGLE_SIZE),
        SCREEN_HEIGHT / 2 - TRIANGLE_MARGIN,

        SCREEN_WIDTH - 1 - (TRIANGLE_SIZE / 2),
        SCREEN_HEIGHT / 2 - TRIANGLE_MARGIN - (TRIANGLE_SIZE),

        SCREEN_WIDTH - 1,
        SCREEN_HEIGHT / 2 - TRIANGLE_MARGIN,

        WHITE
    );

    #undef TRIANGLE_SIZE
    #undef TRIANGLE_MARGIN
}

void Ui::MenuComponent::drawUpdate() {
    Ui::clearRect(
        0,
        SCREEN_HEIGHT - (CHAR_HEIGHT + 2) * 2,
        SCREEN_WIDTH,
        (CHAR_HEIGHT + 2) * 2
    );
    this->drawInitial();
}


void Ui::MenuComponent::reset() {
    activeItems = 0;
    selectedItem = 0;
}

void Ui::MenuComponent::addItem(
    const char* text,
    const unsigned char* icon,
    Ui::MenuHandler handler
) {
    menuItems[activeItems].text = text;
    menuItems[activeItems].icon = icon;
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
