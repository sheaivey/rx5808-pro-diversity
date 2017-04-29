#ifndef STATE_SEARCH_H
#define STATE_SEARCH_H


#include "state.h"
#include "ui_state_menu.h"


#define PEAK_LOOKAHEAD 4


namespace StateMachine {
    class SearchStateHandler : public StateMachine::StateHandler {
        private:
            enum class ScanDirection : int8_t {
                UP = 1,
                DOWN = -1
            };


            bool scanning = false;
            ScanDirection direction = ScanDirection::UP;
            bool forceNext = false;

            bool scanningPeak = false;
            uint8_t peakChannelIndex = 0;
            uint8_t peaks[PEAK_LOOKAHEAD] = { 0 };

            bool menuShowing = true;
            Ui::StateMenuHelper menu = Ui::StateMenuHelper(this);

            void onUpdateAuto();

            void drawBorders();
            void drawChannelText();
            void drawFrequencyText();
            void drawScanBar();
            void drawRssiGraph();
            void drawMenu();

            void setChannel();

        public:
            enum class ScanOrder : uint8_t {
                FREQUENCY,
                CHANNEL
            };


            bool manual = false;
            ScanOrder order = ScanOrder::FREQUENCY;
            uint8_t orderedChanelIndex = 0;

            void onEnter();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange(Button button, Buttons::PressType pressType);
    };
}


#endif
