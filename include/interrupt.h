#ifndef INTERRUPT_H
#define INTERRUPT_H
#include <cstdint>

enum INTERRUPTS: uint8_t {
    VBLANK_INTR = 0,
    STAT_INTR = 1,
    TIMER_INTR = 2,
    SERIAL_INTR = 3,
    JOYPAD_INTR = 4
};

class Interrupt {
public:
    uint8_t IF = 0;
    uint8_t IE = 0;
    uint16_t return_addr[5] = {0x0040, 0x0048, 0x0050, 0x0058, 0x0060};

    uint16_t check_and_service(uint8_t &ime);
    void set_flag(uint8_t bit);
    void reset_flag(uint8_t bit);
    uint8_t is_pending();
};

#endif //INTERRUPT_H
