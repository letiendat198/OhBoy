#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <cstdint>
#include <logger.h>
#include <types.h>

namespace Interrupts {
    inline Logger logger = Logger("Interrupt");

    uint16_t check_and_service(uint8_t &ime);
    void set_interrupt_flag(uint8_t bit);
    void reset_interrupt_flag(uint8_t bit);
    uint8_t is_pending();
};

#endif //INTERRUPTS_H
