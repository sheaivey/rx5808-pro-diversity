#include <avr/pgmspace.h>

#include "state_settings.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "buttons.h"

#include "ui.h"


void StateMachine::SettingsStateHandler::onEnter() {

}

void StateMachine::SettingsStateHandler::onExit() {

}

void StateMachine::SettingsStateHandler::onTick() {

}

void StateMachine::SettingsStateHandler::onButtonChange() {

}


void StateMachine::SettingsStateHandler::onInitialDraw() {
  Ui::clear();
  Ui::needDisplay();
}

void StateMachine::SettingsStateHandler::onUpdateDraw() {

}
