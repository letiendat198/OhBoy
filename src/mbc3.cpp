#include "mbc3.h"

#include <cartridge.h>
#include <chrono>

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
        ram_bank = data & 0xF;
    }
    else if (0x6000 <= addr && addr <= 0x7FFF) {
        if (data == 1 && latch_reg == 0) {
            latch_reg = data;
            uint64_t stored_timestamp = read_stored_timestamp();

            const auto clock = std::chrono::system_clock::now();
            uint64_t current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
            uint64_t time_dif = current_timestamp - stored_timestamp;

            if (stored_timestamp == 0) {
                write_timestamp(current_timestamp);
                return;
            }

            uint64_t stored_sec = read_stored_rtc();
            stored_sec += time_dif;

            write_to_rtc(stored_sec);
            write_timestamp(current_timestamp);
        }
    }
}

uint64_t MBC3::read_stored_timestamp() {
    uint64_t stored_timestamp = 0;
    for (int i=5;i<13;i++) {
        stored_timestamp = (stored_timestamp | Cartridge::external_ram[rtc_offset + i]) << 8;
    }
    return stored_timestamp;
}

void MBC3::write_timestamp(uint64_t timestamp) {
    for (int i=5;i<13;i++) {
        Cartridge::external_ram[rtc_offset+i] = (timestamp >> ((12-i)*8)) & 0xFF;
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
    Cartridge::external_ram[rtc_offset + 4] = (day_high_reg & (0xF - 1)) | ((day & 0x100) >> 9); // Set high day bit
    if (day > 0x1FF) Cartridge::external_ram[rtc_offset + 4] |= 0x80; // Set carry bit
}

uint32_t MBC3::calculate_address(uint16_t addr) {
    if (addr <= 0x3FFF) { // BANK 0
        return addr;
    }
    if (0x4000 <= addr && addr <= 0x7FFF) { // BANK 1 - mapable
        return (rom_bank << 14) | (addr & 0x3FFF);
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (ram_bank < 7 && max_ram_banks > 0) return ((ram_bank % max_ram_banks) << 13) | (addr & 0x1FFF);
        if (rom_bank >= 8) return (0x2000*max_ram_banks + (addr - 0x8)); // Undefined behavior if there's no RAM bank
        return 0x00; // If no RAM present, external ram size is 1;
    }
}
