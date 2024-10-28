#include "cartridge.h"

#include <debugger.h>
#include <format>
#include <memory.h>

bool Cartridge::init(const char* file){
    cartridge_mem = new u_char[0xBFFF+1];
    external_ram = new u_char[0x2000*16];

    // Load bootrom
    f_boot = fopen("../roms/bootix.bin", "rb");
    fseek(f_boot, 0L, SEEK_END);
    boot_size = ftell(f_boot);
    Debugger::log(std::format("BOOT size: {}", boot_size).c_str());
    if(boot_size<=0) return false;

    boot_data = new u_char[0x100+1];
    fseek(f_boot, 0L, SEEK_SET);
    fread(boot_data, sizeof(u_char), 0x100 + 1, f_boot);

    // Load actual rom
    f = fopen(file, "rb");
    fseek(f, 0L, SEEK_END);
    rom_file_size = ftell(f);
    std::cout<<"ROM file size: "<<rom_file_size<<"\n";
    Debugger::log(std::format("ROM size: {}", rom_file_size).c_str());
    if(rom_file_size<=0) return false;

    rom_data = new u_char[rom_file_size];
    fseek(f, 0L, SEEK_SET);
    fread(rom_data, sizeof(u_char),rom_file_size+1, f);
    memcpy(cartridge_mem, rom_data, 0x7FFF+1); // Could f up later

    // Read cartridge metadata
    rom_title = new char[0xF + 1]; // Stub
    strncpy(rom_title, reinterpret_cast<const char *>(rom_data+0x0134), 16);
    mbc_type = read(0x0147);
    rom_size = (32 * (1<<read(0x0148))) / 16;
    ram_size = read(0x0149);
    dest_code = read(0x014A);
    version = read(0x014D);

    std::cout<<"ROM title: "<<rom_title<<"\n";
    std::cout<<std::format("MBC type: {:#X}", mbc_type)<<"\n";
    std::cout<<std::format("ROM size: {:#X}", mbc_type)<<"\n";
    std::cout<<std::format("RAM size: {:#X}", mbc_type)<<"\n";
    std::cout<<std::format("DEST code: {:#X}", mbc_type)<<"\n";
    std::cout<<std::format("VERSION: {:#X}", mbc_type)<<"\n";

    return true;
}

u_char Cartridge::read(u_short addr) {
    if (is_boot && addr<0x100) return boot_data[addr];
    if (0x4000 <= addr && addr <= 0x7FFF) { // If reading VRAM and External RAM
        if (addr+(0x4000*(rom_bank-1)) > rom_file_size) {
            std::cout<<"Trying to read invalid address from rom: "<<addr+(0x4000*(rom_bank-1))<<"\n";
        }
        return rom_data[addr+(0x4000*(rom_bank-1))];
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (ram_bank == 0) return cartridge_mem[addr];
        return external_ram[(addr-0xA000)+(0x2000*(ram_bank-1))];
    }
    return cartridge_mem[addr];
}

void Cartridge::write(u_short addr, u_char data) {
    if (mbc_type == 0x0 && addr <= 0x7FFF) return; // Self-destruct check
    if (mbc_type == 0x01) {
        if (addr <= 0x1FFF) {
            if (data & 0xF == 0xA) ram_enable = true;
            else ram_enable = false;
            return;
        }
        else if (0x2000 <= addr && addr <= 0x3FFF) {
            rom_bank = data & 0x0F; // Depends on ROM presented
            if (rom_bank == 0) rom_bank = 1;
            // std::cout<<"ROM bank switch requested: "<< (int) rom_bank<<"\n";
            return;
            // switch_rom_bank(rom_bank);
        }
        else if (0x4000 <= addr && addr <= 0x5FFF) {
            // std::cout<<"RAM bank switch requested"<<"\n";
            ram_bank = data & 0x3;
            return;
        }
        else if (0x6000 <= addr && addr <= 0x7FFF) {
            bank_mode = data & 0x1;
            return;
        }
        if (0xA000 <= addr && addr <= 0xBFFF) {
            if (ram_bank == 0) cartridge_mem[addr] = data;
            else external_ram[(addr-0xA000)+(0x2000*(ram_bank-1))] = data;
            return;
        }
    }
    cartridge_mem[addr] = data;
}

void Cartridge::switch_rom_bank(u_char rom_bank) {
    rom_bank_offset = 0x4000 * rom_bank;
}


void Cartridge::boot_off() {
    is_boot = false;
}

void Cartridge::close() {
    fclose(f);
}

