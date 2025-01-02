#include "mbc3.h"

#include <cartridge.h>
#include <chrono>
#include <debugger.h>

MBC3::MBC3(uint16_t max_rom_banks, uint16_t max_rom_bank_bit, uint8_t max_ram_banks, uint8_t max_ram_bank_bit) {
    this->max_ram_banks = max_ram_banks;
    this->max_ram_bank_bit = max_ram_bank_bit;
    this->max_rom_banks = max_rom_banks;
    this->max_rom_bank_bit = max_rom_bank_bit;
    rtc_offset = 0x2000 * max_ram_banks;
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
        uint8_t value = data & 0xFF;
        if (value == 0) ram_bank = 0; // In case a 0 RAM ROM want to map back to RAM 0 from RTC
        if (value <= 0x07 && max_ram_banks > 0) ram_bank = value % max_ram_banks;
        else if (0x08 <= value && value <= 0x0C) ram_bank = value;
    }
    else if (0x6000 <= addr && addr <= 0x7FFF) {
        // std::cout<<"Write to latch: "<<(int) data<<"\n";
        // When latch change from 0->1, update register with how much time passed
        // to emulate a ticking clock
        // Auto mode just keep switching from 0->1 to get clock ticking effect
        if (data == 1 && latch_reg == 0) {
            tick_rtc();
        }
        latch_reg = data;
    }
}

uint32_t MBC3::calculate_address(uint16_t addr) {
    if (addr <= 0x3FFF) { // BANK 0
        return addr;
    }
    if (0x4000 <= addr && addr <= 0x7FFF) { // BANK 1 - mapable
        // std::cout<<"ROM bank selected: "<<std::format("{:#X}\n", rom_bank);
        return (rom_bank << 14) | (addr & 0x3FFF);
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (ram_bank <= 7) {
            // std::cout<<"Accessing RAM bank: "<<(int) ram_bank<<"\n";
            return (ram_bank << 13) | (addr & 0x1FFF);
        }
        if (ram_bank >= 8) {
            // std::cout<<"Accessing RTC register: "<<(int) ram_bank<<"\n";
            return (0x2000*max_ram_banks + (ram_bank - 0x8));
        } // Return the specified register
        return 0x00; // If no RAM present, external ram size is 1;
    }
}

void MBC3::tick_rtc() {
    uint64_t stored_timestamp = read_stored_timestamp();

    const auto clock = std::chrono::system_clock::now();
    uint64_t current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
    uint64_t time_dif = current_timestamp - stored_timestamp;

    // std::cout<<"Current timestamp: "<<current_timestamp<<"\n";
    // std::cout<<"Stored timestamp: "<<stored_timestamp<<"\n";

    if (stored_timestamp == 0) {
        write_timestamp(current_timestamp);
        return;
    }

    uint64_t stored_sec = read_stored_rtc();
    stored_sec += time_dif;

    write_to_rtc(stored_sec);
    write_timestamp(current_timestamp);
}


uint64_t MBC3::read_stored_timestamp() {
    uint64_t stored_timestamp = 0;
    for (int i=12;i>=5;i--) {
        stored_timestamp = (stored_timestamp << 8 ) | Cartridge::external_ram[rtc_offset + i] ;
    }
    return stored_timestamp;
}

void MBC3::write_timestamp(uint64_t timestamp) {
    for (int i=5;i<13;i++) {
        Cartridge::external_ram[rtc_offset+i] = (timestamp >> ((i-5)*8)) & 0xFF;
    }
}

uint64_t MBC3::read_stored_rtc() {
    uint32_t to_sec[4] = {1, 60, 3600, 3600*24};
    uint64_t stored_sec = 0;
    for (int i=0;i<4;i++) {
        if (i!=3) stored_sec += Cartridge::external_ram[rtc_offset+i] * to_sec[i];
        else {
            uint8_t day_low = Cartridge::external_ram[rtc_offset+i];
            uint8_t day_high = Cartridge::external_ram[rtc_offset+i+1] & 0x1;
            // std::cout<<"Day low: "<<(int) day_low<<"\n";
            // std::cout<<"Day high: "<<(int) day_high<<"\n";
            stored_sec += ( day_low | (day_high << 8)) * to_sec[i];
        }
    }
    return stored_sec;
}

void MBC3::write_to_rtc(uint64_t sec) {
    Cartridge::external_ram[rtc_offset] = sec % 60;
    Cartridge::external_ram[rtc_offset + 1] = (sec / 60) % 60;
    Cartridge::external_ram[rtc_offset + 2] = (sec / 3600) % 24;
    uint32_t day = sec / (3600*24);
    Cartridge::external_ram[rtc_offset + 3] = day % 0x100;
    uint8_t day_high_reg = Cartridge::external_ram[rtc_offset + 4];
    Cartridge::external_ram[rtc_offset + 4] = (day_high_reg & 0xFE) |  ((day & 0x100) >> 8); // Set high day bit
    if (day > 0x1FF) Cartridge::external_ram[rtc_offset + 4] |= 0x80; // Set carry bit
}