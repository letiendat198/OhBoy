#include "mbc1.h"

#include <iostream>

MBC1::MBC1(uint16_t max_rom_banks, uint16_t max_rom_bank_bit, uint8_t max_ram_banks, uint8_t max_ram_bank_bit) {
    this->max_ram_banks = max_ram_banks;
    this->max_ram_bank_bit = max_ram_bank_bit;
    this->max_rom_banks = max_rom_banks;
    this->max_rom_bank_bit = max_rom_bank_bit;
}

void MBC1::update_registers(uint16_t addr, uint8_t data) {
    if (addr <= 0x1FFF) {
        if ((data & 0xF) == 0xA) ram_enable = true;
        else ram_enable = false;
    }
    else if (0x2000 <= addr && addr <= 0x3FFF) {
        if ((data & 0x1F) == 0) rom_bank_low = 0x01;
        else rom_bank_low = data & 0x1F;
    }
    else if (0x4000 <= addr && addr <= 0x5FFF) {
        uint8_t ram_bank_request = data & 0x3;
        if (max_ram_banks > 0) {
            ram_bank = ram_bank_request % max_ram_banks;
        }
        rom_bank_high = ram_bank_request;
    }
    else if (0x6000 <= addr && addr <= 0x7FFF) {
        bank_mode = data & 0x1;
    }
    rom_bank = ((rom_bank_high << 5) | rom_bank_low) % max_rom_banks;
}

uint32_t MBC1::calculate_address(uint16_t addr) {
    if (addr <= 0x3FFF) { // BANK 0
        if (bank_mode == 0) return addr;
        uint16_t safe_wrap = (rom_bank_high << 5) % max_rom_banks;
        return (safe_wrap << 14) | (addr & 0x3FFF);
    }
    if (0x4000 <= addr && addr <= 0x7FFF) { // BANK 1 - mappable
        return (rom_bank << 14) | (addr & 0x3FFF);
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (bank_mode == 0) return (addr - 0xA000); // Shift to the beginning of array
        return (ram_bank << 13) | (addr & 0x1FFF);
    }
}


