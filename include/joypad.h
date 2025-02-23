#ifndef JOYPAD_H
#define JOYPAD_H
#include <cstdint>

class Joypad {
public:
    uint8_t key_state[8]{}; // Start Select B A Down Up Left Right

    uint8_t input_select = 0;

    void select(uint8_t selection);
    uint8_t read();
};

#endif //JOYPAD_H
