#include "memory.h"
#include <stdlib.h>

bool Memory::read_allow = true;
bool Memory::write_allow = true;
u_char* Memory::memory = (unsigned char*)malloc((0xFFFF+1)*sizeof(char));

u_char Memory::read(u_short addr) {
    if (read_allow && can_read(addr)) {
        return memory[addr];
    }
    else return 0xFF;
}

void Memory::write(u_short addr, u_char data) {
    // if (0xA000 <= addr && addr <= 0xBFFF) {
    //     return;
    // }
    if (write_allow && can_write(addr)) {
        *(memory+addr) = data;
    }
}

u_char *Memory::get_raw() {
    return memory;
}


bool Memory::can_read(u_short addr) {
    return true;
}

bool Memory::can_write(u_short addr) {
    if (addr <= 0x7FFF || (0xE000 <= addr && addr <= 0xFEFF)) {
        return false;
    }
    return true;
}

