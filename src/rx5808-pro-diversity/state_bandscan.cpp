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
    if (!Receiver::isRssiStable())
        return;

    #ifdef USE_DIVERSITY
        rssiData[orderedChanelIndex] = (Receiver::rssiA + Receiver::rssiB) / 2;
    #else
        rssiData[orderedChanelIndex] = Receiver::rssiA;
    #endif

    orderedChanelIndex = (orderedChanelIndex + 1) % (CHANNELS_SIZE);
    Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));

    Ui::needUpdate();

    if (orderedChanelIndex == 0) {

    }
}

#define BORDER_LEFT_X 0
#define BORDER_LEFT_Y 0
#define BORDER_LEFT_H (SCREEN_HEIGHT - CHAR_HEIGHT - 1)

#define BORDER_RIGHT_X (SCREEN_WIDTH - 1)
#define BORDER_RIGHT_Y 0
#define BORDER_RIGHT_H BORDER_LEFT_H

#define BORDER_BOTTOM_X 0
#define BORDER_BOTTOM_Y (SCREEN_HEIGHT - CHAR_HEIGHT - 2)
#define BORDER_BOTTOM_W SCREEN_WIDTH

#define CHANNEL_TEXT_LOW_X 0
#define CHANNEL_TEXT_LOW_Y (SCREEN_HEIGHT - CHAR_HEIGHT)
#define CHANNEL_TEXT_W ((CHAR_WIDTH + 1) * 4)

#define CHANNEL_TEXT_HIGH_X (SCREEN_WIDTH - CHANNEL_TEXT_W + 1)
#define CHANNEL_TEXT_HIGH_Y CHANNEL_TEXT_LOW_Y

#define BORDER_PROGRESS_LEFT_X (CHANNEL_TEXT_LOW_X + CHANNEL_TEXT_W)
#define BORDER_PROGRESS_RIGHT_X (CHANNEL_TEXT_HIGH_X - 2)
#define BORDER_PROGRESS_Y BORDER_BOTTOM_Y
#define BORDER_PROGRESS_H SCREEN_HEIGHT - BORDER_BOTTOM_Y

#define PROGRESS_X (BORDER_PROGRESS_LEFT_X + 2)
#define PROGRESS_Y (BORDER_PROGRESS_Y + 2)
#define PROGRESS_W (BORDER_PROGRESS_RIGHT_X - PROGRESS_X - 1)
#define PROGRESS_H (SCREEN_HEIGHT - PROGRESS_Y) - 2

#define GRAPH_X (BORDER_LEFT_X + 1)
#define GRAPH_Y 0
#define GRAPH_W (BORDER_RIGHT_X - GRAPH_X)
#define GRAPH_H BORDER_BOTTOM_Y


void StateMachine::BandScanStateHandler::onInitialDraw() {
    Ui::clear();

    Ui::display.drawFastVLine(
        BORDER_LEFT_X,
        BORDER_LEFT_Y,
        BORDER_LEFT_H,
        WHITE
    );

    Ui::display.drawFastVLine(
        BORDER_RIGHT_X,
        BORDER_RIGHT_Y,
        BORDER_RIGHT_H,
        WHITE
    );

    Ui::display.drawFastHLine(
        BORDER_BOTTOM_X,
        BORDER_BOTTOM_Y,
        BORDER_BOTTOM_W,
        WHITE
    );

    Ui::display.drawFastHLine(
        BORDER_PROGRESS_LEFT_X,
        SCREEN_HEIGHT - 1,
        BORDER_PROGRESS_RIGHT_X - BORDER_PROGRESS_LEFT_X,
        WHITE
    );

    Ui::display.drawFastVLine(
        BORDER_PROGRESS_LEFT_X,
        BORDER_PROGRESS_Y,
        BORDER_PROGRESS_H,
        WHITE
    );

    Ui::display.drawFastVLine(
        BORDER_PROGRESS_RIGHT_X,
        BORDER_PROGRESS_Y,
        BORDER_PROGRESS_H,
        WHITE
    );

    Ui::display.setTextSize(1);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(CHANNEL_TEXT_LOW_X, CHANNEL_TEXT_LOW_Y);
    Ui::display.print(Channels::getFrequency(Channels::getOrderedIndex(0)));

    Ui::display.setCursor(CHANNEL_TEXT_HIGH_X, CHANNEL_TEXT_HIGH_Y);
    Ui::display.print(
        Channels::getFrequency(Channels::getOrderedIndex(CHANNELS_SIZE - 1)));

    Ui::needDisplay();
}

void StateMachine::BandScanStateHandler::onUpdateDraw() {
    Ui::drawGraph(
        rssiData,
        CHANNELS_SIZE,
        100,
        GRAPH_X,
        GRAPH_Y,
        GRAPH_W,
        GRAPH_H
    );

    Ui::display.drawFastHLine(
        BORDER_BOTTOM_X,
        BORDER_BOTTOM_Y,
        BORDER_BOTTOM_W,
        WHITE
    );

    Ui::clearRect(
        PROGRESS_X,
        PROGRESS_Y,
        PROGRESS_W,
        PROGRESS_H
    );

    uint8_t progressW = orderedChanelIndex * PROGRESS_W / CHANNELS_SIZE + 1;
    Ui::display.fillRect(
        PROGRESS_X,
        PROGRESS_Y,
        progressW,
        PROGRESS_H,
        WHITE
    );

    Ui::needDisplay();
}
