#ifndef TIMER_H
#define TIMER_H
#include <cstdint>

struct TimerControl {
    uint8_t enable = 0;
    uint16_t increment_freq = 0;
    uint8_t bit_select = 0;
};

namespace Timer {
    inline uint32_t div_overflow_cycle = 0;
    inline uint8_t tima = 0;
    inline TimerControl current_tac{};

    uint16_t calc_current_div();
    void schedule_next_div_overflow();
    void schedule_tima_by_div();
    void tick_tima();

    TimerControl read_tac(uint8_t tac);
};

#endif //TIMER_H
