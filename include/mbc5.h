#ifndef MBC5_H
#define MBC5_H
#include <cstdint>

#include "mbc.h"

class MBC5: public MBC {
private:
    uint16_t max_rom_banks;
    uint16_t max_rom_bank_bit;
    uint8_t max_ram_bank_bit;
    uint8_t max_ram_banks;

    uint8_t rom_bank_low = 1;
    uint8_t rom_bank_high = 0;
    uint32_t rom_bank = 1;
    uint8_t ram_bank = 0;
public:
    MBC5(uint16_t max_rom_banks, uint16_t max_rom_bank_bit, uint8_t max_ram_bank, uint8_t max_ram_bank_bit);
    void update_registers(uint16_t addr, uint8_t data) override;
    uint32_t calculate_address(uint16_t addr) override;
};
#endif //MBC5_H
