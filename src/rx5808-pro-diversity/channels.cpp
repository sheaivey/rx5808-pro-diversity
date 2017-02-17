#include <avr/pgmspace.h>

#include "channels.h"
#include "settings.h"


// Channels to sent to the SPI registers
static const uint16_t channelTable[] PROGMEM = {
    0x2A05, 0x299B, 0x2991, 0x2987, 0x291D, 0x2913, 0x2909, 0x289F, // A
    0x2903, 0x290C, 0x2916, 0x291F, 0x2989, 0x2992, 0x299C, 0x2A05, // B
    0x2895, 0x288B, 0x2881, 0x2817, 0x2A0F, 0x2A19, 0x2A83, 0x2A8D, // E
    0x2906, 0x2910, 0x291A, 0x2984, 0x298E, 0x2998, 0x2A02, 0x2A0C, // F / Airwave
    0x281D, 0x288F, 0x2902, 0x2914, 0x2987, 0x2999, 0x2A0C, 0x2A1E  // C / Immersion Raceband
#ifdef USE_LBAND
    ,
    0x2609, 0x261C, 0x268E, 0x2701, 0x2713, 0x2786, 0x2798, 0x280B  // D / 5.3
#endif
};

// Channels with their Mhz Values
static const uint16_t channelFreqTable[] PROGMEM = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F / Airwave
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // C / Immersion Raceband
#ifdef USE_LBAND
    ,
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621  // D / 5.3
#endif
};

// Encode channel names as an 8-bit value where:
//      0b00000111 = channel number (zero-indexed)
//      0b11111000 = channel letter (offset from 'A' character)
static const uint8_t channelNames[] PROGMEM = {
    #define _CHANNEL_NAMES(l) (uint8_t) ((l - 65) << 3)
    #define CHANNEL_NAMES(l) \
        _CHANNEL_NAMES(l) | 0, \
        _CHANNEL_NAMES(l) | 1, \
        _CHANNEL_NAMES(l) | 2, \
        _CHANNEL_NAMES(l) | 3, \
        _CHANNEL_NAMES(l) | 4, \
        _CHANNEL_NAMES(l) | 5, \
        _CHANNEL_NAMES(l) | 6, \
        _CHANNEL_NAMES(l) | 7

    CHANNEL_NAMES('A'),
    CHANNEL_NAMES('B'),
    CHANNEL_NAMES('E'),
    CHANNEL_NAMES('F'), // a.k.a Airwave
    CHANNEL_NAMES('R')  // C / Immersion Raceband

    #ifdef USE_LBAND
        ,
        CHANNEL_NAMES('L')
    #endif

    #undef CHANNEL_NAMES
    #undef _CHANNEL_NAMES
};

// All Channels of the above List ordered by Mhz
static const uint8_t channelFreqOrderedIndex[] PROGMEM = {
#ifdef USE_LBAND
    40, 41, 42, 43, 44, 45, 46, 47, 19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9,
    25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0,
    15, 31, 38, 20, 21, 39, 22, 23
#else
    19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3,
    36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
#endif
};


namespace Channels {
    const uint16_t getSynthRegisterB(uint8_t index) {
        return pgm_read_word_near(channelTable + index);
    }

    const uint16_t getFrequency(uint8_t index) {
        return pgm_read_word_near(channelFreqTable + index);
    }

    // Returns channel name as a string.
    //      dest[] must be at least 3-bytes.
    void getName(uint8_t index, char dest[]) {
        uint8_t encodedName = pgm_read_byte_near(channelNames + index);

        dest[0] = 65 + (encodedName >> 3);
        dest[1] = 48 + (encodedName & (255 >> (8 - 3))) + 1;
        dest[2] = '\0';
    }

    const uint8_t getOrderedIndex(uint8_t index) {
        return pgm_read_byte_near(channelFreqOrderedIndex + index);
    }
}
