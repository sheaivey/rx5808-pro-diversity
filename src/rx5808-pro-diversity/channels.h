#ifndef CHANNELS_H
#define CHANNELS_H

#include <avr/pgmspace.h>

// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
    0x2A05, 0x299B, 0x2991, 0x2987, 0x291D, 0x2913, 0x2909, 0x289F, // A
    0x2903, 0x290C, 0x2916, 0x291F, 0x2989, 0x2992, 0x299C, 0x2A05, // B
    0x2895, 0x288B, 0x2881, 0x2817, 0x2A0F, 0x2A19, 0x2A83, 0x2A8D, // E
    0x2906, 0x2910, 0x291A, 0x2984, 0x298E, 0x2998, 0x2A02, 0x2A0C, // F / Airwave
#ifdef USE_LBAND
    0x281D, 0x288F, 0x2902, 0x2914, 0x2987, 0x2999, 0x2A0C, 0x2A1E,
    0x2609, 0x261C, 0x268E, 0x2701, 0x2713, 0x2786, 0x2798, 0x280B  // D / 5.3
#else
    0x281D, 0x288F, 0x2902, 0x2914, 0x2987, 0x2999, 0x2A0C, 0x2A1E  // C / Immersion Raceband
#endif
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F / Airwave
#ifdef USE_LBAND
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // C / Immersion Raceband
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621  // D / 5.3
#else
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // C / Immersion Raceband
#endif
};

// Do coding as simple hex value to save memory.
const uint8_t channelNames[] PROGMEM = {
    0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // A
    0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, // B
    0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // E
    0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, // F / Airwave
#ifdef USE_LBAND
    0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, // C / Immersion Raceband
    0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8  // D / 5.3
#else
    0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8  // C / Immersion Raceband
#endif
};

// All Channels of the above List ordered by Mhz
const uint8_t channelFreqOrderedIndex[] PROGMEM = {
#ifdef USE_LBAND
    40, 41, 42, 43, 44, 45, 46, 47, 19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9,
    25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0,
    15, 31, 38, 20, 21, 39, 22, 23
#else
    19, 18, 32, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3,
    36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
#endif
};


#endif
