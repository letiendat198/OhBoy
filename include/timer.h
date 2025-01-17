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

    inline uint32_t div_overflow_cycle = 0; // In the past
    inline uint32_t tima_overflow_cycle = 0; // In the future
    inline uint8_t paused_tima = 0;
    inline uint8_t tma = 0;
    inline TimerControl current_tac{};

    uint16_t calc_current_div();
    uint8_t calc_current_tima();
    void schedule_next_div_overflow();
    void schedule_tima_overflow(uint8_t start_value);
    void tick_tima_once();

    TimerControl read_tac(uint8_t tac);
};

#endif //TIMER_H
