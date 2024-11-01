#include "cartridge.h"

#include <debugger.h>
#include <filesystem>
#include <format>
#include <memory.h>

#include "mbc1.h"
#include "mbc5.h"

bool Cartridge::init(const char* file){
    cartridge_mem = new uint8_t[0xBFFF+1]();

    save_name = std::string(file);
    std::size_t delim = save_name.find_last_of('/');
    if (delim == std::string::npos) {
        delim = save_name.find_last_of('\\');
    }
    if (delim != std::string::npos) save_name = save_name.substr(delim+1);
    std::size_t ext_delim = save_name.find_last_of('.');
    save_name = save_name.substr(0, ext_delim) + ".save";

    // Load bootrom
    f_boot = fopen("boot.bin", "rb");
    fseek(f_boot, 0L, SEEK_END);
    boot_size = ftell(f_boot);
    Debugger::log(std::format("BOOT size: {}", boot_size).c_str());
    if(boot_size<=0) {
        std::cerr<<"Boot ROM not found!\n";
        return false;
    }

    boot_data = new uint8_t[0x100+1];
    fseek(f_boot, 0L, SEEK_SET);
    fread(boot_data, sizeof(uint8_t), 0x100 + 1, f_boot);

    // Load actual rom
    f = fopen(file, "rb");
    fseek(f, 0L, SEEK_END);
    rom_file_size = ftell(f);
    std::cout<<"ROM file size: "<<rom_file_size<<"\n";
    Debugger::log(std::format("ROM size: {}", rom_file_size).c_str());
    if(rom_file_size<=0) {
        std::cerr<<"ROM file not found!\n";
        return false;
    }

    rom_data = new uint8_t[rom_file_size];
    fseek(f, 0L, SEEK_SET);
    fread(rom_data, sizeof(uint8_t),rom_file_size+1, f);
    memcpy(cartridge_mem, rom_data, 0x7FFF+1); // Could f up later

    // Read cartridge metadata
    rom_title = new char[0xF + 1]; // Stub
    strncpy(rom_title, reinterpret_cast<const char *>(rom_data+0x0134), 16);

    mbc_type = rom_data[0x0147];
    rom_size = (32 * (1<<rom_data[0x0148]));
    max_rom_banks = rom_size / 16;
    uint8_t temp_size = max_rom_banks - 1;
    while (temp_size!=0) {
        temp_size = temp_size>>1;
        max_rom_bank_bit++;
    }
    ram_size = rom_data[0x0149];
    switch (ram_size) {
        case 0x0:
            max_ram_bank_bit = 0;
            max_ram_banks = 0;
            break;
        case 0x1:
            max_ram_bank_bit = 0;
            max_ram_banks = 0;
            break;
        case 0x2:
            max_ram_bank_bit = 1;
            max_ram_banks = 1;
            break;
        case 0x3:
            max_ram_bank_bit = 2;
            max_ram_banks = 4;
            break;
        case 0x4:
            max_ram_bank_bit = 4;
            max_ram_banks = 16;
            break;
        case 0x5:
            max_ram_bank_bit = 3;
            max_ram_banks = 8;
            break;
    }
    dest_code = rom_data[0x014A];
    version = rom_data[0x014D];

    external_ram_size = max_ram_banks?0x2000*max_ram_banks:1;
    external_ram = new uint8_t[external_ram_size]();
    load_save();

    if (0x1 <= mbc_type && mbc_type <= 0x3) {
        mbc = new MBC1(max_rom_banks, max_rom_bank_bit, max_ram_banks, max_ram_bank_bit);
    }
    else if (0x19 <= mbc_type && mbc_type <= 0x1E) {
        mbc = new MBC5(max_rom_banks, max_rom_bank_bit, max_ram_banks, max_ram_bank_bit);
    }

    if (mbc_type != 0 && mbc == nullptr) {
        std::cerr<<"Unsupported MBC type\n";
        return false;
    }

    std::cout<<"ROM title: "<<rom_title<<"\n";
    std::cout<<std::format("MBC type: {:#X}", mbc_type)<<"\n";
    std::cout<<std::format("ROM banks: {}", max_rom_banks)<<"\n";
    std::cout<<std::format("ROM bank addressing bit needed: {}", max_rom_bank_bit)<<"\n";
    std::cout<<std::format("RAM types: {:#X}", ram_size)<<"\n";
    std::cout<<std::format("RAM banks: {}", max_ram_banks)<<"\n";
    std::cout<<std::format("RAM bank addressing bit needed: {}", max_ram_bank_bit)<<"\n";
    std::cout<<std::format("DEST code: {:#X}", dest_code)<<"\n";
    std::cout<<std::format("VERSION: {:#X}", version)<<"\n";

    return true;
}

uint8_t Cartridge::read(uint16_t addr) {
    if (is_boot && addr<0x100) return boot_data[addr];
    if (addr <= 0x3FFF) {
        if (mbc_type == 0) return rom_data[addr];
        return rom_data[mbc->calculate_address(addr)];
    }
    if (0x4000 <= addr && addr <= 0x7FFF) {
        if (mbc_type == 0) return rom_data[addr];
        return rom_data[mbc->calculate_address(addr)];
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (mbc_type == 0) return external_ram[addr - 0xA000];
        if (!mbc->ram_enable) return 0xFF;
        return external_ram[mbc->calculate_address(addr)];
    }
    return cartridge_mem[addr]; // Only used for vram, move later
}

void Cartridge::write(uint16_t addr, uint8_t data) {
    if (addr <= 0x7FFF) {
        if (mbc_type == 0x0) return; // Self-destruct check
        mbc->update_registers(addr, data);
        return;
    }
    if (0xA000 <= addr && addr <= 0xBFFF) { // Write to External RAM
        if (mbc_type == 0) external_ram[addr - 0xA000] = data;
        if (!mbc->ram_enable) return;
        external_ram[mbc->calculate_address(addr)] = data;
        return;
    }
    cartridge_mem[addr] = data; // Only used for VRAM, remove later
}

void Cartridge::boot_off() {
    is_boot = false;
}

void Cartridge::save_sram() {
    std::cout<<"Saving SRAM to file: "<<"save/"+save_name<<"\n";
    std::filesystem::create_directory("save");
    if(FILE *f_save = fopen(("save/"+save_name).c_str(), "wb")) {
        fwrite(external_ram, sizeof(uint8_t), external_ram_size, f_save);
        fclose(f_save);
    }
    else std::cout<<"Failed to create a save file\n";
}

void Cartridge::load_save() {
    if (FILE *f_save = fopen(("save/"+save_name).c_str(), "rb")) {
        fread(external_ram, sizeof(uint8_t), external_ram_size, f_save);
        fclose(f_save);
    }
    else std::cout<<"No save file for ROM: "<<save_name<<"\n";
}

void Cartridge::close() {
    save_sram();
    fclose(f);
}

