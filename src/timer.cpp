#include "timer.h"

#include <config.h>
#include <memory.h>
#include "interrupts.h"
#include "scheduler.h"

uint16_t Timer::calc_current_div() {
    uint32_t current_cycle = Scheduler::current_cycle;
    if (current_cycle < div_overflow_cycle) current_cycle += CYCLE_PER_FRAME;
    return (current_cycle - div_overflow_cycle);
}

uint32_t Timer::calc_next_tima_overflow(uint8_t current_tima) {
    return (0xFF - current_tima) * current_tac.increment_freq + 3;
}

uint8_t Timer::calc_current_tima() {
    uint32_t current_cycle = Scheduler::current_cycle;
    if (next_tima_overflow_cycle < current_cycle) logger.get_logger()->warn("Current cycle large than TIMA overflow. This shouldn't happen");
    // logger.get_logger()->debug("Current cycle: {:d}, overflow cycle: {:d}, TAC freq: {:d}", current_cycle, next_tima_overflow_cycle, current_tac.increment_freq);
    return 0xFF - ((next_tima_overflow_cycle - current_cycle) / current_tac.increment_freq);
}

void Timer::schedule_next_tima_overflow(uint8_t current_tima) {
    next_tima_overflow_cycle = calc_next_tima_overflow(current_tima);
    Scheduler::schedule(TIMA_OVERFLOW, next_tima_overflow_cycle);
    next_tima_overflow_cycle += Scheduler::current_cycle;
}

TimerControl Timer::read_tac(uint8_t tac) {
    TimerControl control{};
    control.enable = tac >> 2 & 0x1;
    uint8_t clock_select = tac & 0x3;
    uint16_t cycle_delay[4] = {256, 4, 16, 64};
    uint8_t bit_select[4] = {9, 1, 3, 7};
    control.increment_freq = cycle_delay[clock_select];
    control.bit_select = bit_select[clock_select];
    return control;
}

