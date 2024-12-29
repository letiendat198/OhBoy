#ifndef JOYPAD_H
#define JOYPAD_H
#include <cstdint>

class Joypad {
public:
    inline static uint8_t key_state[8]; // Start Select B A Down Up Left Right
    static void tick();
};

#endif //JOYPAD_H
