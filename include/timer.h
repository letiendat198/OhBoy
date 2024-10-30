#ifndef TIMER_H
#define TIMER_H
#include <cstdint>
#include <types.h>

class Timer {
private:
    uint8_t enable;
    uint16_t inc_delay;
    uint16_t cycle;
public:
    void tick();
    void set_timer_control();
};

#endif //TIMER_H
