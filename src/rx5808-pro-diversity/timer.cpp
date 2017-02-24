#include <Arduino.h>
#include "timer.h"


Timer::Timer(uint16_t delay) {
    this->delay = delay;
    this->nextTick = millis() + this->delay;
}

const bool Timer::hasTicked() {
    if (millis() >= this->nextTick)
        return true;

    return false;
}

void Timer::reset() {
    this->nextTick = millis() + this->delay;
}
