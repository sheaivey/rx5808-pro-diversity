#ifndef STATE_SEARCH_H
#define STATE_SEARCH_H


#include "state.h"


#define PEAK_LOOKAHEAD 4


namespace StateMachine {
    class SearchStateHandler : public StateMachine::StateHandler {
        private:
            enum class ScanDirection : int8_t {
                UP = 1,
                DOWN = -1
            };


            bool manual = false;
            bool scanning = false;
            ScanDirection direction = ScanDirection::UP;
            bool forceNext = false;
            uint8_t orderedChanelIndex = 0;

            bool scanningPeak = false;
            uint8_t peakChannelIndex = 0;
            uint8_t peaks[PEAK_LOOKAHEAD] = { 0 };


            void onUpdateAuto();
            void onUpdateManual();

            void drawBorders();
            void drawChannelText();
            void drawFrequencyText();
            void drawScanBar();
            void drawRssiGraph();


        public:
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange(Button button, Buttons::PressType pressType);
    };
}


#endif
