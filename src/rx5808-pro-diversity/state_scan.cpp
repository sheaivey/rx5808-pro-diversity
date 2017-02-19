#include <avr/pgmspace.h>

#include "state_scan.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "buttons.h"

#include "ui.h"
#include "ui_menu.h"


static uint8_t orderedChanelIndex = 0;
static uint8_t lastChannelIndex = 0;

static uint8_t rssiData[CHANNELS_SIZE] = { 0 };


static void onButtonChange();


void StateMachine::ScanStateHandler::onEnter() {
    orderedChanelIndex = 0;
    lastChannelIndex = Receiver::activeChannel;

    ButtonState::registerChangeFunc(onButtonChange);
}

void StateMachine::ScanStateHandler::onExit() {
    Receiver::setChannel(lastChannelIndex);

    ButtonState::deregisterChangeFunc(onButtonChange);
}


void StateMachine::ScanStateHandler::onTick() {
    Receiver::waitForStableRssi();
    rssiData[orderedChanelIndex] = (Receiver::rssiA + Receiver::rssiB) / 2;

    orderedChanelIndex = (orderedChanelIndex + 1) % (CHANNELS_SIZE);
    uint8_t realChannelIndex = Channels::getOrderedIndex(
        orderedChanelIndex);

    Receiver::setChannel(realChannelIndex);
    Ui::needUpdate();
}


static void onButtonChange() {
    if (ButtonState::get(Button::MODE)) {
        StateMachine::switchState(StateMachine::State::MENU);
    }
}


void StateMachine::ScanStateHandler::onInitialDraw() {

}

void StateMachine::ScanStateHandler::onUpdateDraw() {
    Ui::clear();

    Ui::drawGraph(
        rssiData,
        CHANNELS_SIZE,
        100,
        2,
        0,
        SCREEN_WIDTH - 3,
        SCREEN_HEIGHT
    );

    Ui::drawDashedVLine(
        orderedChanelIndex * (SCREEN_WIDTH / CHANNELS_SIZE),
        0,
        SCREEN_HEIGHT,
        4
    );

    Ui::display.drawFastVLine(
        0,
        0,
        SCREEN_HEIGHT,
        WHITE
    );

    Ui::display.drawFastVLine(
        SCREEN_WIDTH - 1,
        0,
        SCREEN_HEIGHT,
        WHITE
    );

    Ui::display.setTextSize(1);
    Ui::display.setTextColor(INVERSE);
    Ui::display.setCursor(2, SCREEN_HEIGHT - CHAR_HEIGHT);
    Ui::display.print(Channels::getFrequency(Channels::getOrderedIndex(0)));

    Ui::display.setCursor(
        SCREEN_WIDTH - 2 - (CHAR_WIDTH + 1) * 4,
        SCREEN_HEIGHT - CHAR_HEIGHT);
    Ui::display.print(
        Channels::getFrequency(Channels::getOrderedIndex(CHANNELS_SIZE - 1)));

    Ui::needDisplay();
}
