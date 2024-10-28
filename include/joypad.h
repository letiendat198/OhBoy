#ifndef JOYPAD_H
#define JOYPAD_H
#include <types.h>

class Joypad {
public:
    inline static u_char key_state[8]; // Start Select B A Down Up Left Right
    static void tick();
};

#endif //JOYPAD_H
