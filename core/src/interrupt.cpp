#include "interrupt.h"

uint16_t Interrupt::check_and_service(uint8_t &ime) { // Need to make sure that IE & IF != 0 and ime != 0
    uint8_t interrupt_status = IE & IF;
    for (int i=0;i<=4;i++) {
        uint8_t flag = (interrupt_status >> i) & 0x1;
        if (flag == 1) {
            ime = 0;
            reset_flag(i);
            return return_addr[i];
        }
    }
    return 0x00;
}

uint8_t Interrupt::is_pending() {
    return (IE & IF) != 0;
}

void Interrupt::set_flag(uint8_t bit) {
    IF = IF | 0x1<<bit ;
}

void Interrupt::reset_flag(uint8_t bit) {
    IF = IF & ~(0x1<<bit);
}


