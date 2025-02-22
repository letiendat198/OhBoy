#include "timer.h"

#include <config.h>
#include "scheduler.h"

uint16_t Timer::calc_current_div() {
    uint32_t current_cycle = Scheduler::current_cycle;
    if (current_cycle < div_overflow_cycle) current_cycle += CYCLE_PER_FRAME;
    return (current_cycle - div_overflow_cycle);
}

void Timer::schedule_next_div_overflow() {
    Scheduler::schedule(DIV_OVERFLOW, DIV_OVERFLOW_CYCLE);
    div_overflow_cycle = Scheduler::current_cycle;
}

void Timer::schedule_tima_overflow(uint8_t start_value) {
    uint16_t current_div = calc_current_div();
    tima_overflow_cycle = (0x100 - start_value) * current_tac.increment_freq - (current_div % current_tac.increment_freq);

    Scheduler::reschedule(TIMA_OVERFLOW, tima_overflow_cycle);
    tima_overflow_cycle += Scheduler::current_cycle;
}

uint8_t Timer::calc_current_tima() {
    return 0xFF - ((tima_overflow_cycle - 1 - Scheduler::current_cycle) / current_tac.increment_freq);
}

void Timer::tick_tima_once() { // Potentially problematic
    tima_overflow_cycle -= current_tac.increment_freq;
    Scheduler::remove_schedule(TIMA_OVERFLOW);
    Scheduler::schedule_absolute(TIMA_OVERFLOW, tima_overflow_cycle);
}

TimerControl Timer::read_tac(uint8_t tac) {
    TimerControl control{};
    control.enable = tac >> 2 & 0x1;
    uint8_t clock_select = tac & 0x3;
    uint16_t cycle_delay[4] = {256, 4, 16, 64};
    uint8_t bit_select[4] = {7, 1, 3, 5};
    control.increment_freq = cycle_delay[clock_select];
    control.bit_select = bit_select[clock_select];
    return control;
}

