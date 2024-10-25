#ifndef TIMER_H
#define TIMER_H
#include <types.h>

class Timer {
private:
    u_char enable;
    u_short inc_delay;
    u_short cycle;
public:
    void tick();
    void set_timer_control();
};

#endif //TIMER_H
