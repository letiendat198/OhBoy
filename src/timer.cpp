#include "timer.h"

#include <memory.h>

#include "interrupts.h"

void Timer::tick() {
    set_timer_control();
    cycle++;
    if (cycle % 64 == 0) {
        uint8_t div = (Memory::read(0xFF04) + 1) % 0x100;
        Memory::unsafe_write(0xFF04, div);
    }
    if (cycle % inc_delay == 0 && enable) {
        uint8_t tima = (Memory::read(0xFF05) + 1) % 0x100;
        if (tima == 0) {
            Interrupts::set_if(2);
            tima = Memory::read(0xFF06);
        }
        Memory::write(0xFF05, tima);
    }
    if (cycle == 256) cycle = 0;
}


void Timer::set_timer_control() {
    uint8_t tac = Memory::read(0xFF07);
    enable = (tac  & 0x4) >> 2;
    uint8_t clock_select = tac & 0x3;
    switch (clock_select) {
        case 0:
            inc_delay = 256;
            break;
        case 1:
            inc_delay = 4;
            break;
        case 2:
            inc_delay = 16;
            break;
        case 3:
            inc_delay = 64;
            break;
    }
}

