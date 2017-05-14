#ifndef STATE_MENU_H
#define STATE_MENU_H


#include "state.h"
#include "ui_menu.h"


namespace StateMachine {
    class MenuStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange(Button button, Buttons::PressType pressType);

        private:
            void drawMenuEntry();
            Ui::MenuHelper menu;
    };
}


#endif
