#include "state.h"
#include "state_menu.h"
#include "ui.h"
#include "ui_menu.h"
#include "buttons.h"


static void onButtonChange();

static void searchMenuHandler();
static void bandScannerMenuHandler();
static void settingsMenuHandler();


void StateMachine::MenuStateHandler::onEnter() {
    ButtonState::registerChangeFunc(onButtonChange);
}

void StateMachine::MenuStateHandler::onExit() {
    ButtonState::deregisterChangeFunc(onButtonChange);
}


void StateMachine::MenuStateHandler::onInitialDraw() {
    Ui::clear();

    Ui::drawTitle("Main Menu");

    Ui::menu.reset();
    Ui::menu.addItem("Search", searchMenuHandler);
    Ui::menu.addItem("Band Scanner", bandScannerMenuHandler);
    Ui::menu.addItem("Settings", settingsMenuHandler);

    Ui::menu.drawInitial();
    Ui::needDisplay();
}

void StateMachine::MenuStateHandler::onUpdateDraw() {
    Ui::menu.drawUpdate();
    Ui::needDisplay();
}


static void onButtonChange() {
    if (ButtonState::get(Button::UP)) {
        Ui::menu.selectPreviousItem();
        Ui::needUpdate();
    }

    if (ButtonState::get(Button::DOWN)) {
        Ui::menu.selectNextItem();
        Ui::needUpdate();
    }

    if (ButtonState::get(Button::MODE)) {
        Ui::menu.activateItem();
    }
}

static void searchMenuHandler() {
    StateMachine::switchState(StateMachine::State::AUTO);
};

static void bandScannerMenuHandler() {
    StateMachine::switchState(StateMachine::State::SCAN);
};

static void settingsMenuHandler() {
    StateMachine::switchState(StateMachine::State::SCAN);
};

