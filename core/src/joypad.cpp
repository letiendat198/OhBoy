#include "joypad.h"

void Joypad::select(uint8_t selection) {
    input_select = selection;
}

uint8_t Joypad::read() {
    uint8_t joypad_state = 0;
    if ((input_select & 0x1) == 0) { // Select dpad
        for (uint8_t i = 0;i<=3;i++) {
            joypad_state = joypad_state << 1 | ~key_state[4+i] & 0x1;
        }
    }
    else if (((input_select>>1) & 0x1) == 0) { // Select buttons
        for (uint8_t i = 0;i<=3;i++) {
            joypad_state = joypad_state << 1 | ~key_state[i] & 0x1;
        }
    }
    else joypad_state = joypad_state << 4 | 0xF;
    return (input_select << 4) | (joypad_state & 0xF);
}


