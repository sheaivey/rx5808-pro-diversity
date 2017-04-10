#include <avr/pgmspace.h>

#include "state_settings.h"

#include "state.h"
#include "buttons.h"
#include "ui.h"

#include "pstr_helper.h"


void StateMachine::SettingsStateHandler::onEnter() {

}

void StateMachine::SettingsStateHandler::onExit() {

}

void StateMachine::SettingsStateHandler::onUpdate() {

}

void StateMachine::SettingsStateHandler::onButtonChange(
    Button button,
    Buttons::PressType pressType
) {
    if (button == Button::MODE) {
        StateMachine::switchState(StateMachine::State::SETTINGS_RSSI);
    }
}


void StateMachine::SettingsStateHandler::onInitialDraw() {
    Ui::clear();

    Ui::display.setTextSize(1);
    Ui::display.setCursor(0, 0);
    Ui::display.print(PSTR2("Press mode for\nRSSI calibration"));

    Ui::needDisplay();
}

void StateMachine::SettingsStateHandler::onUpdateDraw() {

}
