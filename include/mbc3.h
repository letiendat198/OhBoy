#ifndef MBC3_H
#define MBC3_H
#include <cstdint>

#include "mbc.h"

class MBC3: public MBC {
private:
    uint16_t max_rom_banks;
    uint16_t max_rom_bank_bit;
    uint8_t max_ram_bank_bit;
    uint8_t max_ram_banks;
    uint32_t rtc_offset;

    uint8_t rom_bank=1;
    uint8_t ram_bank = 0;
    uint8_t latch_reg = 1;

    void tick_rtc();
    uint64_t read_stored_rtc();
    void write_to_rtc(uint64_t sec);
    uint64_t read_stored_timestamp();
    void write_timestamp(uint64_t timestamp);
public:
    MBC3(uint16_t max_rom_banks, uint16_t max_rom_bank_bit, uint8_t max_ram_bank, uint8_t max_ram_bank_bit);
    void update_registers(uint16_t addr, uint8_t data) override;
    uint32_t calculate_address(uint16_t addr) override;
};
#endif //MBC3_H
