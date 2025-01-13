#ifndef JOYPAD_H
#define JOYPAD_H
#include <cstdint>

namespace Joypad {

    inline uint8_t key_state[8]; // Start Select B A Down Up Left Right
    void tick();
};

#endif //JOYPAD_H
