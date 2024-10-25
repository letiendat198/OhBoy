#include "interrupts.h"

#include <memory.h>

u_short Interrupts::check_and_service(u_char &ime){
    if (ime==0) {
        return 0x00;
    }
    u_char ie = Memory::read(0xFFFF);
    u_char iflag = Memory::read(0xFF0F);
    u_char istatus = ie & iflag;
    for (int i=0;i<=4;i++) {
        u_char idata = (istatus & 0x1<<i) >> i;
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

u_char Interrupts::is_pending() {
    u_char ie = Memory::read(0xFFFF);
    u_char iflag = Memory::read(0xFF0F);
    u_char istatus = ie & iflag;
    return istatus != 0;
}


void Interrupts::set_if(u_char bit) {
    u_char iflag = Memory::read(0xFF0F);
    Memory::write(0xFF0F, iflag | 0x1<<bit);
}

void Interrupts::reset_if(u_char bit) {
    u_char iflag = Memory::read(0xFF0F);
    Memory::write(0xFF0F, iflag & ~(0x1<<bit));
}


