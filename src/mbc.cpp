#include <cartridge.h>
#include <mbc.h>

MBC::MBC(Cartridge *cartridge) {
    this->cartridge = cartridge;
}

void MBC::update_registers(uint16_t addr, uint8_t data) {
    switch (mbc_type) {
        case MBC0:
            return;
        case MBC1: {
            if (addr <= 0x1FFF) {
                if ((data & 0xF) == 0xA) cartridge->ram_enable = true;
                else cartridge->ram_enable = false;
            }
            else if (0x2000 <= addr && addr <= 0x3FFF) {
                if ((data & 0x1F) == 0) rom_bank_low = 0x01;
                else rom_bank_low = data & 0x1F;
            }
            else if (0x4000 <= addr && addr <= 0x5FFF) {
                uint8_t ram_bank_request = data & 0x3;
                if (cartridge->header.ram_banks > 0) {
                    ram_bank = ram_bank_request % cartridge->header.ram_banks;
                }
                rom_bank_high = ram_bank_request;
            }
            else if (0x6000 <= addr && addr <= 0x7FFF) {
                bank_mode = data & 0x1;
            }
            cartridge->rom_bank = ((rom_bank_high << 5) | rom_bank_low) % cartridge->header.rom_banks;
            if (bank_mode == 1) cartridge->ram_bank = ram_bank;
            else cartridge->ram_bank = 0;
            break;
        }
        case MBC3: {
            if (addr <= 0x1FFF) {
                if ((data & 0xF) == 0xA) cartridge->ram_enable = true;
                else cartridge->ram_enable = false;
            }
            else if (0x2000 <= addr && addr <= 0x3FFF) {
                if ((data & 0x7F) == 0) cartridge->rom_bank = 1;
                else cartridge->rom_bank = (data & 0x7F) % cartridge->header.rom_banks;
            }
            else if (0x4000 <= addr && addr <= 0x5FFF) {
                uint8_t value = data & 0xFF;
                // if (value == 0) cartridge->ram_bank_ptr = 0; // In case a 0 RAM ROM want to map back to RAM 0 from RTC
                if (value <= 0x07 && cartridge->header.ram_banks > 0) cartridge->ram_bank = value % cartridge->header.ram_banks;
                // else if (0x08 <= value && value <= 0x0C) cartridge->ram_bank_ptr = value;
            }
            else if (0x6000 <= addr && addr <= 0x7FFF) {
                // std::cout<<"Write to latch: "<<(int) data<<"\n";
                // When latch change from 0->1, update register with how much time passed
                // to emulate a ticking clock
                // Auto mode just keep switching from 0->1 to get clock ticking effect
                // if (data == 1 && latch_reg == 0) {
                //     tick_rtc();
                // }
                // latch_reg = data;
            }
            break;
        }
        case MBC5: {
            if (addr <= 0x1FFF) {
                if ((data & 0xF) == 0xA) cartridge->ram_enable = true;
                else cartridge->ram_enable = false;
            }
            else if (0x2000 <= addr && addr <= 0x2FFF) {
                rom_bank_low = data & 0xFF;
            }
            else if (0x3000 <= addr && addr <= 0x3FFFF) {
                rom_bank_high = data & 0x1;
            }
            else if (0x4000 <= addr && addr <= 0x5FFF) {
                if (cartridge->header.ram_banks > 0) cartridge->ram_bank = (data & 0xF) % cartridge->header.ram_banks;
            }
            cartridge->rom_bank = ((rom_bank_high << 8) | rom_bank_low) % cartridge->header.rom_banks;
            break;
        }
    }
}


