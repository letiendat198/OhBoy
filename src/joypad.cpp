#include "joypad.h"

#include <memory.h>

void Joypad::tick() {
    uint8_t joypad_control = (Memory::unsafe_read(0xFF00) >> 4) & 0x3;
    uint8_t joypad_state = 0;
    if ((joypad_control & 0x1) == 0) { // Select dpad
        // std::cout<<"Reading from DPAD\n";
        for (uint8_t i = 0;i<=3;i++) {
            joypad_state = joypad_state << 1 | !key_state[4+i];
        }
    }
    else if (((joypad_control>>1) & 0x1) == 0) { // Select buttons
        // std::cout<<"Reading from buttons\n";
        for (uint8_t i = 0;i<=3;i++) {
            joypad_state = joypad_state << 1 | !key_state[i];
        }
    }
    else {
        // std::cout<<"Read nothing\n";
        joypad_state = joypad_state << 4 | 0xF;
    }
    Memory::unsafe_write(0xFF00, joypad_control << 4 | joypad_state);
}

