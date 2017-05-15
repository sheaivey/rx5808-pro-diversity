#include "state_search.h"

#include "receiver.h"
#include "channels.h"
#include "ui.h"
#include "pstr_helper.h"


#define BORDER_GRAPH_L_X 59

#define CHANNEL_TEXT_SIZE 5
#define CHANNEL_TEXT_X 0
#define CHANENL_TEXT_Y 0
#define CHANNEL_TEXT_H (CHAR_HEIGHT * CHANNEL_TEXT_SIZE)

#define FREQUENCY_TEXT_SIZE 2
#define FREQUENCY_TEXT_X 6
#define FREQUENCY_TEXT_Y (SCREEN_HEIGHT - (CHAR_HEIGHT * 2))
#define FREQUENCY_TEXT_H (CHAR_HEIGHT * FREQUENCY_TEXT_SIZE)

#define SCANBAR_BORDER_X 0
#define SCANBAR_BORDER_Y (CHANNEL_TEXT_H + 4)
#define SCANBAR_BORDER_W (BORDER_GRAPH_L_X - 4)
#define SCANBAR_BORDER_H 7

#define SCANBAR_X (SCANBAR_BORDER_X + 2)
#define SCANBAR_Y (SCANBAR_BORDER_Y + 2)
#define SCANBAR_W (SCANBAR_BORDER_W - 4)
#define SCANBAR_H (SCANBAR_BORDER_H - 4)

#define GRAPH_SEPERATOR_Y SCREEN_HEIGHT_MID
#define GRAPH_SEPERATOR_W (SCREEN_WIDTH - BORDER_GRAPH_L_X)
#define GRAPH_SEPERATOR_STEP 3

#define GRAPH_X (BORDER_GRAPH_L_X + 2)
#define GRAPH_W (SCREEN_WIDTH - BORDER_GRAPH_L_X)
#ifdef USE_DIVERSITY
    #define GRAPH_H (GRAPH_SEPERATOR_Y - 2)
    #define GRAPH_A_Y 0
    #define GRAPH_B_Y (SCREEN_HEIGHT - GRAPH_H - 1)

    #define RX_TEXT_SIZE 1
    #define RX_TEXT_X (BORDER_GRAPH_L_X + 4)
    #define RX_TEXT_H (CHAR_HEIGHT * RX_TEXT_SIZE)
    #define RX_TEXT_A_Y ((GRAPH_A_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
    #define RX_TEXT_B_Y ((GRAPH_B_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
#else
    #define GRAPH_H (SCREEN_HEIGHT - 1)
    #define GRAPH_Y 0
    #define GRAPH_B_Y 0
#endif


using Ui::display;


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
        BORDER_GRAPH_L_X,
        CHANNEL_TEXT_H
    );

    Ui::clearRect(
        0,
        FREQUENCY_TEXT_Y,
        BORDER_GRAPH_L_X,
        CHAR_HEIGHT * 2
    );

    Ui::clearRect(
        SCANBAR_X,
        SCANBAR_Y,
        SCANBAR_W,
        SCANBAR_H
    );

    drawChannelText();
    drawFrequencyText();
    drawScanBar();
    drawRssiGraph();
    menu.draw();

    Ui::needDisplay();
}

void StateMachine::SearchStateHandler::drawBorders() {
    display.drawRoundRect(
        SCANBAR_BORDER_X,
        SCANBAR_BORDER_Y,
        SCANBAR_BORDER_W,
        SCANBAR_BORDER_H,
        2,
        WHITE
    );

    Ui::drawDashedVLine(
        BORDER_GRAPH_L_X,
        0,
        SCREEN_HEIGHT,
        GRAPH_SEPERATOR_STEP
    );
}

void StateMachine::SearchStateHandler::drawChannelText() {
    display.setTextSize(CHANNEL_TEXT_SIZE);
    display.setTextColor(WHITE);
    display.setCursor(CHANNEL_TEXT_X, CHANENL_TEXT_Y);

    display.print(Channels::getName(Receiver::activeChannel));
}

void StateMachine::SearchStateHandler::drawFrequencyText() {
    display.setTextSize(FREQUENCY_TEXT_SIZE);
    display.setTextColor(WHITE);
    display.setCursor(FREQUENCY_TEXT_X, FREQUENCY_TEXT_Y);

    display.print(Channels::getFrequency(Receiver::activeChannel));
}

void StateMachine::SearchStateHandler::drawScanBar() {
    uint8_t scanWidth = orderedChanelIndex * SCANBAR_W / CHANNELS_SIZE;

    display.fillRect(
        SCANBAR_X,
        SCANBAR_Y,
        scanWidth,
        SCANBAR_H,
        WHITE
    );
}

void StateMachine::SearchStateHandler::drawRssiGraph() {
    #ifdef USE_DIVERSITY
        Ui::drawGraph(
            Receiver::rssiBLast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_A_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawGraph(
            Receiver::rssiALast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_B_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawDashedHLine(
            GRAPH_X,
            GRAPH_SEPERATOR_Y,
            GRAPH_SEPERATOR_W,
            GRAPH_SEPERATOR_STEP
        );

        display.setTextSize(RX_TEXT_SIZE);
        display.setTextColor(INVERSE);

        display.setCursor(RX_TEXT_X, RX_TEXT_A_Y);
        display.print(PSTR2("B"));

        display.setCursor(RX_TEXT_X, RX_TEXT_B_Y);
        display.print(PSTR2("A"));
    #else
        Ui::drawGraph(
            Receiver::rssiALast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_Y,
            GRAPH_W,
            GRAPH_H
        );
    #endif
}

void StateMachine::SearchStateHandler::drawMenu() {
    this->menu.draw();
}
