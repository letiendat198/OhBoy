#ifndef INTERRUPT_H
#define INTERRUPT_H
#include <cstdint>
#include <logger.h>

enum INTERRUPTS: uint8_t {
    VBLANK_INTR = 0,
    STAT_INTR = 1,
    TIMER_INTR = 2,
    SERIAL_INTR = 3,
    JOYPAD_INTR = 4
};

namespace Interrupt {
    inline Logger logger = Logger("Interrupt");

    inline uint8_t IF = 0;
    inline uint8_t IE = 0;
    inline uint16_t return_addr[5] = {0x0040, 0x0048, 0x0050, 0x0058, 0x0060};

    uint16_t check_and_service(uint8_t &ime);
    void set_flag(uint8_t bit);
    void reset_flag(uint8_t bit);
    uint8_t is_pending();
};

#endif //INTERRUPT_H
