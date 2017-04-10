#ifndef RECEIVER_H
#define RECEIVER_H


#include <stdint.h>
#include "settings.h"


#define RECEIVER_A 0
#ifdef USE_DIVERSITY
  #define RECEIVER_B 1
#endif

#define RECEIVER_LAST_DELAY 50
#define RECEIVER_LAST_DATA_SIZE 24


namespace Receiver {
    extern uint8_t activeReceiver;
    extern uint8_t activeChannel;

    extern uint8_t rssiA;
    extern uint16_t rssiARaw;
    extern uint8_t rssiALast[RECEIVER_LAST_DATA_SIZE];
    #ifdef USE_DIVERSITY
        extern uint8_t rssiB;
        extern uint16_t rssiBRaw;
        extern uint8_t rssiBLast[RECEIVER_LAST_DATA_SIZE];
    #endif

    void setChannel(uint8_t channel);
    void waitForStableRssi();
    uint16_t updateRssi();
    void setActiveReceiver(uint8_t receiver = RECEIVER_A);
    #ifdef USE_DIVERSITY
        void setDiversityMode(uint8_t mode);
        void switchDiversity();
    #endif

    #ifdef USE_DIVERSITY
        enum class DiversityMode : uint8_t {
            AUTO,
            FORCE_A,
            FORCE_B
        };
    #endif

    void setup();
    void update();
}


#endif
