#ifndef RECEIVER_H
#define RECEIVER_H


#include <stdint.h>
#include "settings.h"


#define RECEIVER_A 0
#ifdef USE_DIVERSITY
  #define RECEIVER_B 1
  #define RECEIVER_AUTO 2

  #define DIVERSITY_AUTO 0
  #define DIVERSITY_FORCE_A 1
  #define DIVERSITY_FORCE_B 2
#endif


namespace Receiver {
    extern uint8_t activeReceiver;
    extern uint8_t activeChannel;
    extern uint8_t rssiA;
    #ifdef USE_DIVERSITY
        extern uint8_t rssiB;
    #endif

    void setChannel(uint8_t channel);
    void waitForStableRssi();
    uint16_t updateRssi();
    void setActiveReceiver(uint8_t receiver = RECEIVER_A);
    #ifdef USE_DIVERSITY
        void setDiversityMode(uint8_t mode);
        void switchDiversity();
    #endif

    void update();
}


#endif
