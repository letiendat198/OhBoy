#include "interrupts.h"

#include <memory.h>

uint16_t Interrupts::check_and_service(uint8_t &ime){
    uint8_t interrupt_enable = Memory::read(0xFFFF);
    uint8_t interrupt_flag = Memory::read(0xFF0F);
    uint8_t interrupt_status = interrupt_enable & interrupt_flag;
    for (int i=0;i<=4;i++) {
        uint8_t idata = (interrupt_status & 0x1<<i) >> i;
        if (idata == 1) {
            ime = 0;
            reset_interrupt_flag(i);
            uint8_t return_addr[5] = {0x40, 0x48, 0x50, 0x58, 0x60};
            return return_addr[i];
        }
    }
    return 0x00;
}

uint8_t Interrupts::is_pending() {
    uint8_t interrupt_enable = Memory::read(0xFFFF);
    uint8_t interrupt_flag = Memory::read(0xFF0F);
    uint8_t interrupt_status = interrupt_enable & interrupt_flag;
    return interrupt_status != 0;
}

void Interrupts::set_interrupt_flag(uint8_t bit) {
    uint8_t interrupt_flag = Memory::read(0xFF0F);
    Memory::write(0xFF0F, interrupt_flag | 0x1<<bit);
}

void Interrupts::reset_interrupt_flag(uint8_t bit) {
    uint8_t interrupt_flag = Memory::read(0xFF0F);
    Memory::write(0xFF0F, interrupt_flag & ~(0x1<<bit));
}


