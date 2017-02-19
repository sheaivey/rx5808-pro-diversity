#include <Arduino.h>

#include "state_auto.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "buttons.h"
#include "ui.h"


enum class ScanDirection : int8_t {
    UP = 1,
    DOWN = -1
};


static bool scanning = true;
static ScanDirection direction = ScanDirection::UP;
static bool forceNext = false;

static void onButtonChange();


void StateMachine::AutoStateHandler::onEnter() {
    ButtonState::registerChangeFunc(onButtonChange);
}

void StateMachine::AutoStateHandler::onExit() {
    ButtonState::deregisterChangeFunc(onButtonChange);
}

void StateMachine::AutoStateHandler::onTick() {
    if (scanning) {
        if (!forceNext && Receiver::rssiA >= RSSI_SEEK_TRESHOLD)
            scanning = false;

        int nextChannel = Receiver::activeChannel
            + static_cast<int8_t>(direction);
        if (nextChannel < 0)
            nextChannel = CHANNELS_SIZE - 1;
        if (nextChannel >= CHANNELS_SIZE)
            nextChannel = 0;

        Receiver::setChannel(nextChannel);

        if (forceNext)
            forceNext = false;
    }

    Ui::needUpdate();
}


static void onButtonChange() {
    if (ButtonState::get(Button::UP)) {
        scanning = true;
        forceNext = true;
        direction = ScanDirection::UP;
    } else if (ButtonState::get(Button::DOWN)) {
        scanning = true;
        forceNext = true;
        direction = ScanDirection::DOWN;
    } else if (ButtonState::get(Button::MODE)) {
        StateMachine::switchState(StateMachine::State::MENU);
    }
}


static void drawBorders();
static void drawChannelText();
static void drawFrequencyText();
static void drawScanBar();
static void drawRssiGraph();


void StateMachine::AutoStateHandler::onInitialDraw() {
    Ui::clear();

    drawBorders();

    drawChannelText();
    drawFrequencyText();
    drawScanBar();
    drawRssiGraph();

    Ui::needDisplay();
}

void StateMachine::AutoStateHandler::onUpdateDraw() {
    Ui::clearRect(
        0,
        0,
        59,
        CHAR_HEIGHT * 5
    );

    Ui::clearRect(
        0,
        SCREEN_HEIGHT - (CHAR_HEIGHT * 2),
        59,
        CHAR_HEIGHT * 2
    );

    Ui::clearRect(
        1,
        (CHAR_HEIGHT * 5) + 4 + 1,
        54,
        5
    );

    drawChannelText();
    drawFrequencyText();
    drawScanBar();
    drawRssiGraph();

    Ui::needDisplay();
}


static void drawBorders() {
    Ui::display.drawFastVLine(59, 0, SCREEN_HEIGHT, WHITE);
    Ui::display.drawFastVLine(SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT, WHITE);

    Ui::display.drawRoundRect(
        0,
        (CHAR_HEIGHT * 5) + 4,
        56,
        7,
        2,
        WHITE
    );
}

static void drawChannelText() {
    Ui::display.setTextSize(5);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(0, 0);

    char channelName[3];
    Channels::getName(Receiver::activeChannel, channelName);

    Ui::display.print(channelName);
}

static void drawFrequencyText() {
    Ui::display.setTextSize(2);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(6, SCREEN_HEIGHT - (CHAR_HEIGHT * 2));

    Ui::display.print(Channels::getFrequency(Receiver::activeChannel));
}

static void drawScanBar() {
    int scanWidth = Receiver::activeChannel * 54 / CHANNELS_SIZE;

    Ui::display.fillRect(
        1,
        (CHAR_HEIGHT * 5) + 4 + 1,
        scanWidth,
        5,
        WHITE
    );
}

static void drawRssiGraph() {
    Ui::drawGraph(
        Receiver::rssiALast,
        RECEIVER_LAST_DATA_SIZE,
        100,
        62,
        0,
        66,
        30
    );

    Ui::drawGraph(
        Receiver::rssiBLast,
        RECEIVER_LAST_DATA_SIZE,
        100,
        62,
        34,
        66,
        30
    );

    Ui::drawDashedHLine(60, 32, 64, 8);

    if (Receiver::activeReceiver == RECEIVER_A) {
        Ui::display.fillRoundRect(
            59,
            7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    } else {
        Ui::display.fillRoundRect(
            59,
            7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            BLACK
        );

        Ui::display.drawRoundRect(
            59,
            7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    }

    if (Receiver::activeReceiver == RECEIVER_B) {
        Ui::display.fillRoundRect(
            59,
            32 + 7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    } else {
        Ui::display.fillRoundRect(
            59,
            7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            BLACK
        );

        Ui::display.drawRoundRect(
            59,
            32 + 7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    }

    Ui::display.setTextColor(INVERSE);

    Ui::display.setCursor(61, 16 - CHAR_HEIGHT);
    Ui::display.print("A");

    Ui::display.setCursor(61, 48 - CHAR_HEIGHT);
    Ui::display.print("B");
}
