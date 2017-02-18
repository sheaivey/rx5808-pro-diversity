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
    int scanWidth = Receiver::activeChannel * 54 / (CHANNELS_SIZE - 1);

    Ui::display.fillRect(
        1,
        (CHAR_HEIGHT * 5) + 4 + 1,
        scanWidth,
        5,
        WHITE
    );
}

static void drawGraph(
    const uint8_t data[],
    const uint8_t dataSize,
    const uint8_t dataScale,
    const int x,
    const int y,
    const int w,
    const int h
) {
    #define SCALE_DATAPOINT(p) (p * h / dataScale)
    #define CLAMP_DATAPOINT(p) \
        (p > dataScale) ? dataScale : ((p < 0) ? 0 : p);

    Ui::clearRect(x, y, w - 1, h + 1);

    const uint8_t xScaler = w / dataSize;
    const uint8_t xScalarMissing = w % dataSize;

    for (uint8_t i = 0; i < dataSize - 1; i++) {
        const uint8_t dataPoint = CLAMP_DATAPOINT(data[i]);
        const uint8_t dataPointNext = CLAMP_DATAPOINT(data[i + 1]);

        // Need to invert the heights so it shows the right way on he screen.
        const uint8_t dataPointHeight = h - SCALE_DATAPOINT(dataPoint);
        const uint8_t dataPointNextHeight = h - SCALE_DATAPOINT(dataPointNext);

        Ui::display.drawLine(
            x + (i) * xScaler,
            y + dataPointHeight,
            x + (i + 1) * xScaler + (i == dataSize - 2 ? xScalarMissing : 0),
            y + dataPointNextHeight,
            WHITE
        );
    }

    #undef SCALE_DATAPOINT
    #undef CLAMP_DATAPOINT
}

static void drawRssiGraph() {
    drawGraph(
        Receiver::rssiALast,
        RECEIVER_LAST_DATA_SIZE,
        100,
        62,
        0,
        66,
        30
    );

    drawGraph(
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
