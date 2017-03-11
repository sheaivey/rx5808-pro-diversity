#include "state_search.h"

#include "receiver.h"
#include "channels.h"
#include "ui.h"
#include "pstr_helper.h"


void StateMachine::SearchStateHandler::onInitialDraw() {
    Ui::clear();

    drawBorders();

    drawChannelText();
    drawFrequencyText();
    drawScanBar();
    drawRssiGraph();

    Ui::needDisplay();
}

void StateMachine::SearchStateHandler::onUpdateDraw() {
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

    /*Ui::display.setTextSize(1);
    Ui::display.setTextColor(WHITE);

    char channelName[3];
    Channels::getName(Receiver::activeChannel, channelName);

    Ui::display.setCursor(0, 0);
    Ui::display.print(channelName[0]);
    Ui::display.setCursor(0, CHAR_HEIGHT + 1);
    Ui::display.print(channelName[1]);

    String freq = String(Channels::getFrequency(Receiver::activeChannel));
    Ui::display.setCursor(0, SCREEN_HEIGHT - ((CHAR_HEIGHT + 1) * 4) + 1);
    Ui::display.print(freq.charAt(0));
    Ui::display.setCursor(0, SCREEN_HEIGHT - ((CHAR_HEIGHT + 1) * 3) + 1);
    Ui::display.print(freq.charAt(1));
    Ui::display.setCursor(0, SCREEN_HEIGHT - ((CHAR_HEIGHT + 1) * 2) + 1);
    Ui::display.print(freq.charAt(2));
    Ui::display.setCursor(0, SCREEN_HEIGHT - ((CHAR_HEIGHT + 1) * 1) + 1);
    Ui::display.print(freq.charAt(3));

    Ui::display.drawFastVLine(CHAR_WIDTH + 1, 0, SCREEN_HEIGHT, WHITE);
    Ui::display.drawFastHLine(0, (CHAR_HEIGHT + 1) * 2, CHAR_WIDTH + 1, WHITE);
    Ui::display.drawFastHLine(0, ((CHAR_HEIGHT + 1) * 4) - 2, CHAR_WIDTH + 1, WHITE);

    uint8_t barMaxHeight = (((CHAR_HEIGHT + 1) * 4) - 2) - ((CHAR_HEIGHT + 1) * 2) - 2;
    uint8_t barHeight = orderedChanelIndex * barMaxHeight / CHANNELS_SIZE;
    Ui::display.fillRect(0, (CHAR_HEIGHT + 1) * 2 + 2, CHAR_WIDTH, barHeight, WHITE);

    Ui::drawDashedHLine(CHAR_WIDTH + 2, 32, SCREEN_WIDTH - (CHAR_WIDTH + 2), 8);

    Ui::drawGraph(
        Receiver::rssiALast,
        RECEIVER_LAST_DATA_SIZE,
        100,
        CHAR_WIDTH + 2,
        0,
        SCREEN_WIDTH - (CHAR_WIDTH + 2),
        30
    );

    Ui::drawGraph(
        Receiver::rssiBLast,
        RECEIVER_LAST_DATA_SIZE,
        100,
        CHAR_WIDTH + 2,
        SCREEN_HEIGHT - 30,
        SCREEN_WIDTH - (CHAR_WIDTH + 2),
        30
    );*/

    Ui::needDisplay();
}


void StateMachine::SearchStateHandler::drawBorders() {
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

void StateMachine::SearchStateHandler::drawChannelText() {
    Ui::display.setTextSize(5);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(0, 0);

    Ui::display.print(Channels::getName(Receiver::activeChannel));
}

void StateMachine::SearchStateHandler::drawFrequencyText() {
    Ui::display.setTextSize(2);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(6, SCREEN_HEIGHT - (CHAR_HEIGHT * 2));

    Ui::display.print(Channels::getFrequency(Receiver::activeChannel));
}

void StateMachine::SearchStateHandler::drawScanBar() {
    uint8_t scanWidth = orderedChanelIndex * 54 / CHANNELS_SIZE;

    Ui::display.fillRect(
        1,
        (CHAR_HEIGHT * 5) + 4 + 1,
        scanWidth,
        5,
        WHITE
    );
}

void StateMachine::SearchStateHandler::drawRssiGraph() {
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
            32 + 7,
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
    Ui::display.print(PSTR2("A"));

    Ui::display.setCursor(61, 48 - CHAR_HEIGHT);
    Ui::display.print(PSTR2("B"));
}
