#ifndef TIMER_H
#define TIMER_H
#include <cstdint>
#include <logger.h>

struct TimerControl {
    uint8_t enable;
    uint16_t increment_freq;
};

class Timer {
public:
    Logger logger = Logger("Timer");
    void tick_div();
    void tick_tima();
    TimerControl read_tac();
};

#endif //TIMER_H
