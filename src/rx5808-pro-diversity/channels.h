#ifndef CHANNELS_H
#define CHANNELS_H


namespace Channels {
    const uint16_t getSynthRegisterB(uint8_t index);
    const uint16_t getFrequency(uint8_t index);
    void getName(uint8_t index, char dest[]);
    const uint8_t getOrderedIndex(uint8_t index);
}


#endif
