#ifndef UI_MENU_H


#include "ui.h"


namespace Ui {
    typedef void(*MenuHandler)();

    class MenuComponent : public UiComponent {
        public:
            void drawInitial();
            void drawUpdate();

            void reset();
            void addItem(
                const char* text,
                const unsigned char* icon,
                const MenuHandler handler
            );

            void selectNextItem();
            void selectPreviousItem();
            void activateItem();
    };

    extern MenuComponent menu;
}


#endif
