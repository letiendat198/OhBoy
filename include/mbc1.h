#ifndef MBC1_H
#define MBC1_H

#include <cstdint>
#include "mbc.h"

class MBC1: public MBC {
private:
    uint16_t max_rom_banks;
    uint16_t max_rom_bank_bit;
    uint8_t max_ram_bank_bit;
    uint8_t max_ram_banks;

    uint16_t rom_bank = 1;
    uint8_t ram_bank = 0;
    uint8_t bank_mode = 0;
    uint8_t rom_bank_low = 1; // Need to be 1 initially to map to bank 1, otherwise we gonna get 0x39 garbage
    uint8_t rom_bank_high = 0;
public:
    MBC1(uint16_t max_rom_banks, uint16_t max_rom_bank_bit, uint8_t max_ram_bank, uint8_t max_ram_bank_bit);
    void update_registers(uint16_t addr, uint8_t data) override;
    uint32_t calculate_address(uint16_t addr) override;
};

#endif //MBC1_H
