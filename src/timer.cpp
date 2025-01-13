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

void Timer::schedule_next_div_overflow() {
    uint16_t current_div = calc_current_div();
    if (((current_div >> current_tac.bit_select) & 0x1) == 1) tick_tima();

    Scheduler::schedule(DIV_OVERFLOW, DIV_OVERFLOW_CYCLE);
    div_overflow_cycle = Scheduler::current_cycle;

    Scheduler::reschedule(TIMA_TICK, current_tac.increment_freq);
}

void Timer::schedule_tima_by_div() {
    uint16_t current_div = calc_current_div();
    uint16_t delay = current_tac.increment_freq;
    // logger.get_logger()->debug("Rescheduling TIMA tick at cycle: {:d}, current DIV: {:d}, delay: {:d}, next tick: {:d}",
    //         Scheduler::current_cycle, current_div, delay, delay - (current_div % delay));
    Scheduler::reschedule(TIMA_TICK, delay - (current_div % delay));
}


void Timer::tick_tima() {
    if (current_tac.enable) {
        // logger.get_logger()->debug("Tick TIMA. Current TIMA: {:d}+1. Current cycle: {:d}. DIV: {:b}", tima, Scheduler::current_cycle, calc_current_div());
        tima = (tima + 1) & 0xFF;
        if (tima == 0) {
            // logger.get_logger()->debug("Timer interrupt request at cycle: {:d}", Scheduler::current_cycle);
            Interrupts::set_interrupt_flag(2);
        }
    }
    Scheduler::schedule(TIMA_TICK, current_tac.increment_freq);
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

