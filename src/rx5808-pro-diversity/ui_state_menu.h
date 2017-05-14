#ifndef UI_STATE_MENU_H
#define UI_STATE_MENU_H


#include "buttons.h"
#include "ui.h"


#define MENU_ITEMS_MAX 4


namespace Ui {
    class StateMenuHelper {
        public:
            typedef const unsigned char* (*MenuIcon)(void* state);
            typedef void (*MenuHandler)(void* state);


            struct StateMenuItem {
                MenuIcon icon = nullptr;
                MenuHandler handler = nullptr;
            };


            StateMenuHelper(void* state) { this->state = state; }
            void draw();
            bool handleButtons(Button button, Buttons::PressType pressType);
            bool isVisible() { return this->visible; };
            void addItem(
                const MenuIcon icon,
                const MenuHandler handler
            );


        private:
            StateMenuItem menuItems[MENU_ITEMS_MAX];

            void *state = nullptr;
            int activeItems = 0;
            int selectedItem = 0;
            bool visible = false;

            int8_t slideX = 0;
    };
}


#endif
