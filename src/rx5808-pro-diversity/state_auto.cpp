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
    //drawScreen.seekMode(STATE_SEEK);
}

void StateMachine::AutoStateHandler::onExit() {
    ButtonState::deregisterChangeFunc(onButtonChange);
}

void StateMachine::AutoStateHandler::onTick() {
    /*drawScreen.updateSeekMode(
        STATE_SEEK,
        Receiver::activeChannel,
        Channels::getOrderedIndex(Receiver::activeChannel),
        Receiver::rssiA,
        Channels::getFrequency(Receiver::activeChannel),
        RSSI_SEEK_TRESHOLD,
        !scanning
    );*/

    if (scanning) {
        if (!forceNext && Receiver::rssiA >= RSSI_SEEK_TRESHOLD)
            scanning = false;

        int nextChannel = Receiver::activeChannel
            + static_cast<int8_t>(direction);
        if (nextChannel < 0)
            nextChannel = CHANNEL_MAX_INDEX;
        if (nextChannel > CHANNEL_MAX_INDEX)
            nextChannel = 0;

        Receiver::setChannel(nextChannel);

        if (forceNext)
            forceNext = false;

        Ui::needUpdate();
    }
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
    }
}



static void drawChannelText();
static void drawFrequencyText();
static void drawScanBar();
static void drawRssiGraph();


void StateMachine::AutoStateHandler::onInitialDraw() {
    Ui::clear();

    Ui::display.drawFastVLine(59, 0, SCREEN_HEIGHT, WHITE);
    Ui::display.drawFastVLine(60, 0, SCREEN_HEIGHT, WHITE);
    Ui::display.drawFastVLine(61, 0, SCREEN_HEIGHT, WHITE);
    Ui::display.drawFastVLine(SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT, WHITE);

    //Ui::display.drawFastHLine(60, 32, SCREEN_WIDTH - 60, WHITE);

    Ui::display.drawRoundRect(
        0,
        (CHAR_HEIGHT * 5) + 4,
        56,
        7,
        2,
        WHITE
    );

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

    Ui::clearRect(
        62,
        0,
        SCREEN_WIDTH - 62 - 1,
        SCREEN_HEIGHT
    );

    drawChannelText();
    drawFrequencyText();
    drawScanBar();
    drawRssiGraph();

    Ui::needDisplay();
}

static void drawChannelText() {
    Ui::display.setTextSize(5);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(0, 0);
    Ui::display.print(Channels::getName(Receiver::activeChannel), HEX);
}

static void drawFrequencyText() {
    Ui::display.setTextSize(2);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(6, SCREEN_HEIGHT - (CHAR_HEIGHT * 2));

    Ui::display.print(Channels::getFrequency(Receiver::activeChannel));
}

static void drawScanBar() {
    float scanPercent = Receiver::activeChannel / (float) CHANNEL_MAX_INDEX;

    Ui::display.fillRect(
        1,
        (CHAR_HEIGHT * 5) + 4 + 1,
        54 * scanPercent,
        5,
        WHITE
    );
}

static void drawRssiGraph() {
    for (uint8_t i = 1; i < 16; i++) {
        float barPercentLastA = Receiver::rssiALast[i - 1] / 100.0f;
        int barHeightLastA = 30 * barPercentLastA / 2;

        float barPercentA = Receiver::rssiALast[i] / 100.0f;
        int barHeightA = 30 * barPercentA / 2;

        Ui::display.drawLine(
            (60 + 4) + ((i - 1) * 4),
            32 - (30 - barHeightLastA),
            (60 + 4) + ((i) * 4),
            32 - (30 - barHeightA),
            WHITE
        );

        float barPercentLastB = Receiver::rssiBLast[i - 1] / 100.0f;
        int barHeightLastB = 30 * barPercentLastB;

        float barPercentB = Receiver::rssiBLast[i] / 100.0f;
        int barHeightB = 30 * barPercentB;

        Ui::display.drawLine(
            (60 + 4) + ((i - 1) * 4),
            SCREEN_HEIGHT - (30 - barHeightLastB),
            (60 + 4) + ((i) * 4),
            SCREEN_HEIGHT -  (30 - barHeightB),
            WHITE
        );
    }

    for (int i = 0; i <= 64; i += 4) {
         Ui::display.drawFastHLine(60 + i, 32, 2, WHITE);
    }

    Ui::display.setTextSize(2);

    if (Receiver::activeReceiver == RECEIVER_A) {
        Ui::display.fillRoundRect(
            61,
            7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    } else {
        Ui::display.drawRoundRect(
            61,
            7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    }

    if (Receiver::activeReceiver == RECEIVER_B) {
        Ui::display.fillRoundRect(
            61,
            32 + 7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    } else {
        Ui::display.drawRoundRect(
            61,
            32 + 7,
            CHAR_WIDTH * 2 + 2 + 2,
            32 - 7 - 7,
            2,
            WHITE
        );
    }

    Ui::display.setTextColor(INVERSE);

    Ui::display.setCursor(63, 16 - CHAR_HEIGHT);
    Ui::display.print("A");

    Ui::display.setCursor(63, 48 - CHAR_HEIGHT);
    Ui::display.print("B");
}
