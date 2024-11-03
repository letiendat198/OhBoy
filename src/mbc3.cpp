#include "mbc3.h"

MBC3::MBC3(uint16_t max_rom_banks, uint16_t max_rom_bank_bit, uint8_t max_ram_banks, uint8_t max_ram_bank_bit) {
    this->max_ram_banks = max_ram_banks;
    this->max_ram_bank_bit = max_ram_bank_bit;
    this->max_rom_banks = max_rom_banks;
    this->max_rom_bank_bit = max_rom_bank_bit;
}

void MBC3::update_registers(uint16_t addr, uint8_t data) {
    if (addr <= 0x1FFF) {
        if ((data & 0xF) == 0xA) ram_enable = true;
        else ram_enable = false;
    }
    else if (0x2000 <= addr && addr <= 0x3FFF) {
        if ((data & 0x7F) == 0) rom_bank = 1;
        else rom_bank = (data & 0x7F) % max_rom_banks;
    }
    else if (0x4000 <= addr && addr <= 0x5FFF) {
        if (max_ram_banks > 0) ram_bank = (data & 0x7) % max_ram_banks;
    }
}

uint32_t MBC3::calculate_address(uint16_t addr) {
    if (addr <= 0x3FFF) { // BANK 0
        return addr;
    }
    if (0x4000 <= addr && addr <= 0x7FFF) { // BANK 1 - mapable
        return (rom_bank << 14) | (addr & 0x3FFF);
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        return (ram_bank << 13) | (addr & 0x1FFF);
    }
}
