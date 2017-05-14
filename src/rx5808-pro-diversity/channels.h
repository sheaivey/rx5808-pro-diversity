#ifndef CHANNELS_H
#define CHANNELS_H


#include "settings.h"


#ifdef USE_LBAND
    #define CHANNELS_SIZE 48
#else
    #define CHANNELS_SIZE 40
#endif


namespace Channels {
    const uint16_t getSynthRegisterB(uint8_t index);
    const uint16_t getFrequency(uint8_t index);
    const char *getName(uint8_t index);
    const uint8_t getOrderedIndex(uint8_t index);
    const uint8_t getOrderedIndexFromIndex(uint8_t index);
}


#endif
