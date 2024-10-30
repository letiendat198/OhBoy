#ifndef MBC_H
#define MBC_H
#include <types.h>

class MBC {
public:
    bool ram_enable;
    virtual void update_registers(uint16_t addr, uint8_t data) = 0;
    virtual uint32_t calculate_address(uint16_t addr) = 0;
};

#endif //MBC_H
