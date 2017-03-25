#include <avr/pgmspace.h>

#include "state_bandscan.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "buttons.h"

#include "ui.h"
#include "ui_menu.h"


void StateMachine::BandScanStateHandler::onEnter() {
    orderedChanelIndex = 0;
    lastChannelIndex = Receiver::activeChannel;
}

void StateMachine::BandScanStateHandler::onExit() {
    Receiver::setChannel(lastChannelIndex);
}


void StateMachine::BandScanStateHandler::onUpdate() {
    Receiver::waitForStableRssi();

    #ifdef USE_DIVERSITY
        rssiData[orderedChanelIndex] = (Receiver::rssiA + Receiver::rssiB) / 2;
    #else
        rssiData[orderedChanelIndex] = Receiver::rssiA;
    #endif

    orderedChanelIndex = (orderedChanelIndex + 1) % (CHANNELS_SIZE);
    Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));

    Ui::needUpdate();
}


void StateMachine::BandScanStateHandler::onInitialDraw() {

}

void StateMachine::BandScanStateHandler::onUpdateDraw() {
    Ui::clear();

    Ui::drawGraph(
        rssiData,
        CHANNELS_SIZE,
        100,
        1,
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
