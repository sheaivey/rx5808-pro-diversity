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

void StateMachine::SettingsStateHandler::onButtonChange() {
    if (Buttons::get(Button::MODE)->pressed) {
        StateMachine::switchState(StateMachine::State::SETTINGS_RSSI);
    }
}


void StateMachine::SettingsStateHandler::onInitialDraw() {
  Ui::clear();
  Ui::needDisplay();
}

void StateMachine::SettingsStateHandler::onUpdateDraw() {

}
