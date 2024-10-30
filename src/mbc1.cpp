#include "mbc1.h"

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
        if ((data & 0x1F) == 0) rom_bank = 0x01;
        else rom_bank = (data & 0x1F) % max_rom_banks;
        // std::cout<<(int) rom_bank<<"\n";
        if (max_rom_bank_bit > 5) rom_bank = ((rom_bank_ext << 5) | rom_bank) % max_rom_banks;
        // std::cout<<(int) data<<"\n";
        // std::cout<<"ROM bank switch requested: "<< std::format("{:#X}", rom_bank_reg)<<"\n";
    }
    else if (0x4000 <= addr && addr <= 0x5FFF) {
        uint8_t ram_bank_request = data & 0x3;
        // std::cout<<"2-bit register data: " <<(int) ram_bank_request<<"\n";
        if (max_ram_banks > 0) {
            ram_bank = ram_bank_request % max_ram_banks;
            // std::cout<<"RAM bank switch requested: "<<(int) ram_bank<<"\n";
        }
        rom_bank_ext = ram_bank_request;
    }
    else if (0x6000 <= addr && addr <= 0x7FFF) {
        bank_mode = data & 0x1; // TODO: Implement this annoying thing
    }
}

uint32_t MBC1::calculate_address(uint16_t addr) {
    if (addr <= 0x3FFF) { // BANK 0
        return addr;
        // return (rom_bank_ext<<18) | addr; Might overflow, implement later
    }
    if (0x4000 <= addr && addr <= 0x7FFF) { // BANK 1 - mapable
        return (rom_bank << 14) | (addr & 0x3FFF);
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (bank_mode == 0) return (addr - 0xA000); // Shift to the beginning of array
        return (ram_bank << 13) | (addr & 0x1FFF);
    }
}


