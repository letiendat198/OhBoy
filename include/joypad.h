#ifndef JOYPAD_H
#define JOYPAD_H
#include <cstdint>
#include <logger.h>

class Joypad {
private:
    inline static Logger logger = Logger("Joypad");
public:
    inline static uint8_t key_state[8]; // Start Select B A Down Up Left Right
    static void tick();
};

#endif //JOYPAD_H
