#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <cstdint>
#include <types.h>

class Interrupts {
private:

public:
    static uint16_t check_and_service(uint8_t &ime);
    static void set_interrupt_flag(uint8_t bit);
    static void reset_interrupt_flag(uint8_t bit);
    static uint8_t is_pending();
};

#endif //INTERRUPTS_H
