#ifndef TIMER_H
#define TIMER_H
#include <cstdint>
#include <logger.h>

struct TimerControl {
    uint8_t enable = 0;
    uint16_t increment_freq = 0;
    uint8_t bit_select = 0;
};

namespace Timer {
    inline Logger logger = Logger("Timer");

    inline bool enable = false;

    inline uint32_t div_overflow_cycle = 0;
    inline uint32_t next_tima_overflow_cycle = 0;
    inline uint8_t paused_tima_value = 0;
    inline TimerControl current_tac{};

    uint16_t calc_current_div();

    void tick_tima();

    void schedule_next_tima_overflow(uint8_t current_tima);

    TimerControl read_tac(uint8_t tac);
};

#endif //TIMER_H
