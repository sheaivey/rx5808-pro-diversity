#ifndef UI_MENU_H


#include "ui.h"


namespace Ui {
    typedef void(*MenuHandler)();

    struct MenuItem {
        const char* text = nullptr;
        Ui::MenuHandler handler = nullptr;
        const unsigned char* icon = nullptr;
    };

    class MenuHelper {
        public:
            void reset();
            void addItem(
                const char* text,
                const unsigned char* icon,
                const MenuHandler handler
            );

            MenuItem* getCurrentItem();

            void selectNextItem();
            void selectPreviousItem();
            void activateItem();
    };

    extern MenuHelper menu;
}


#endif
