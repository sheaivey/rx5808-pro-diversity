#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdint.h>
#include "settings.h"

#define RECEIVER_A 0
#ifdef USE_DIVERSITY
  #define RECEIVER_B 1
  #define RECEIVER_AUTO 2
#endif

extern uint8_t activeReceiver;

void setChannel(uint8_t channel);
uint16_t readRssi(uint8_t receiver = RECEIVER_A);
uint16_t setActiveReceiver(uint8_t receiver = RECEIVER_A);

#endif
