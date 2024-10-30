#include "interrupts.h"

#include <memory.h>

uint16_t Interrupts::check_and_service(uint8_t &ime){
    if (ime==0) {
        return 0x00;
    }
    uint8_t ie = Memory::read(0xFFFF);
    uint8_t iflag = Memory::read(0xFF0F);
    uint8_t istatus = ie & iflag;
    for (int i=0;i<=4;i++) {
        uint8_t idata = (istatus & 0x1<<i) >> i;
        if (idata == 1) {
            ime = 0;
            reset_if(i);
            switch (i) {
                case 0:
                    return 0x40;
                case 1:
                    return 0x48;
                case 2:
                    return 0x50;
                case 3:
                    return 0x58;
                case 4:
                    return 0x60;
            }
        }
    }
    return 0x00;
}

uint8_t Interrupts::is_pending() {
    uint8_t ie = Memory::read(0xFFFF);
    uint8_t iflag = Memory::read(0xFF0F);
    uint8_t istatus = ie & iflag;
    return istatus != 0;
}


void Interrupts::set_if(uint8_t bit) {
    uint8_t iflag = Memory::read(0xFF0F);
    Memory::write(0xFF0F, iflag | 0x1<<bit);
}

void Interrupts::reset_if(uint8_t bit) {
    uint8_t iflag = Memory::read(0xFF0F);
    Memory::write(0xFF0F, iflag & ~(0x1<<bit));
}


