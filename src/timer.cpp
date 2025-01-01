#include "timer.h"

#include <memory.h>

#include "interrupts.h"
#include "scheduler.h"

void Timer::tick_div() {
    uint8_t div = (Memory::read(0xFF04) + 1) % 0x100;
    Memory::unsafe_write(0xFF04, div);
    Scheduler::schedule(DIV_TICK, 64);
}


void Timer::tick_tima() {
    TimerControl tac = read_tac();
    if (tac.enable) {
        uint8_t tima = (Memory::read(0xFF05) + 1) % 0x100;
        if (tima == 0) {
            Interrupts::set_interrupt_flag(2);
            tima = Memory::read(0xFF06);
        }
        Memory::write(0xFF05, tima);
    }
    Scheduler::schedule(TIMA_TICK, tac.increment_freq); // Timer won't reshedule until it tick again -> Shit implementation
}


TimerControl Timer::read_tac() {
    TimerControl control{};
    uint8_t tac = Memory::read(0xFF07);
    control.enable = (tac  & 0x4) >> 2;
    uint8_t clock_select = tac & 0x3;
    switch (clock_select) {
        case 0:
            control.increment_freq = 256;
            break;
        case 1:
            control.increment_freq = 4;
            break;
        case 2:
            control.increment_freq = 16;
            break;
        case 3:
            control.increment_freq = 64;
            break;
    }
    return control;
}

