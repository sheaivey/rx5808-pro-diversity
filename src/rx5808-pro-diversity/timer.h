#ifndef TIMER_H
#define TIMER_H


#include <stdint.h>


class Timer {
    private:
        uint32_t nextTick;
        uint16_t delay;

    public:
        Timer(uint16_t delay);
        const bool hasTicked();
        void reset();
};


#endif
