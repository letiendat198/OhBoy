#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <cstdint>
#include <logger.h>
#include <types.h>

namespace Interrupts {
    inline Logger logger = Logger("Interrupt");

    inline uint8_t IF = 0;
    inline uint8_t IE = 0;
    inline uint16_t return_addr[5] = {0x0040, 0x0048, 0x0050, 0x0058, 0x0060};

    uint16_t check_and_service(uint8_t &ime);
    void set_interrupt_flag(uint8_t bit);
    void reset_interrupt_flag(uint8_t bit);
    uint8_t is_pending();
};

#endif //INTERRUPTS_H
