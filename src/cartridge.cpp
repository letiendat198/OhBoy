#include "cartridge.h"

#include <debugger.h>
#include <filesystem>
#include <format>
#include <memory.h>

#include "mbc1.h"
#include "mbc3.h"
#include "mbc5.h"


std::string get_save_file_name(std::string file_name) {
    std::size_t delim = file_name.find_last_of('/');
    if (delim == std::string::npos) {
        delim = file_name.find_last_of('\\');
    }
    if (delim != std::string::npos) file_name = file_name.substr(delim+1);
    std::size_t ext_delim = file_name.find_last_of('.');
    file_name = file_name.substr(0, ext_delim) + ".sav";
    return file_name;
}

bool Cartridge::read_rom(const char* file) {
    // Load actual rom
    f = fopen(file, "rb");
    fseek(f, 0L, SEEK_END);
    rom_file_size = ftell(f);

    if(rom_file_size<=0) {
        std::cerr<<"ROM file not found!\n";
        return false;
    }

    rom_data = new uint8_t[rom_file_size];
    fseek(f, 0L, SEEK_SET);
    fread(rom_data, sizeof(uint8_t),rom_file_size+1, f);
    return true;
}

RomMetadata Cartridge::read_rom_metadata() {
    RomMetadata metadata{};
    metadata.rom_title = new char[0xF + 1]; // Stub
    strncpy(metadata.rom_title, reinterpret_cast<const char *>(rom_data+0x0134), 16);

    metadata.mbc_type = rom_data[0x0147];
    metadata.rom_size = (32 * (1<<rom_data[0x0148]));
    metadata.max_rom_banks = metadata.rom_size / 16;
    uint8_t temp_size = metadata.max_rom_banks - 1;
    while (temp_size!=0) {
        temp_size = temp_size>>1;
        metadata.max_rom_bank_bit++;
    }
    metadata.ram_size = rom_data[0x0149];
    switch (metadata.ram_size) {
        case 0x0:
        case 0x1:
            metadata.max_ram_bank_bit = 0;
            metadata.max_ram_banks = 0;
            break;
        case 0x2:
            metadata.max_ram_bank_bit = 1;
            metadata.max_ram_banks = 1;
            break;
        case 0x3:
            metadata.max_ram_bank_bit = 2;
            metadata.max_ram_banks = 4;
            break;
        case 0x4:
            metadata.max_ram_bank_bit = 4;
            metadata.max_ram_banks = 16;
            break;
        case 0x5:
            metadata.max_ram_bank_bit = 3;
            metadata.max_ram_banks = 8;
            break;
    }
    metadata.dest_code = rom_data[0x014A];
    metadata.version = rom_data[0x014D];
    metadata.cgb_flag = rom_data[0x143];

    return metadata;
}

bool Cartridge::load_boot (bool is_cgb) {
    f_boot = fopen("boot.bin", "rb");
    if (is_cgb) f_boot = fopen("cgb_boot.bin", "rb");
    fseek(f_boot, 0L, SEEK_END);
    boot_size = ftell(f_boot);
    logger.get_logger()->debug(std::format("BOOT size: {}", boot_size).c_str());
    if(boot_size<=0) {
        std::cerr<<"Boot ROM not found!\n";
        return false;
    }

    boot_data = new uint8_t[boot_size];
    fseek(f_boot, 0L, SEEK_SET);
    fread(boot_data, sizeof(uint8_t), boot_size, f_boot);
    return true;
}

void log_metadata(RomMetadata metadata) {
    std::cout<<"ROM title: "<<metadata.rom_title<<"\n";
    std::cout<<std::format("MBC type: {:#X}", metadata.mbc_type)<<"\n";
    std::cout<<std::format("ROM banks: {}", metadata.max_rom_banks)<<"\n";
    std::cout<<std::format("ROM bank addressing bit needed: {}", metadata.max_rom_bank_bit)<<"\n";
    std::cout<<std::format("RAM types: {:#X}", metadata.ram_size)<<"\n";
    std::cout<<std::format("RAM banks: {}", metadata.max_ram_banks)<<"\n";
    std::cout<<std::format("RAM bank addressing bit needed: {}", metadata.max_ram_bank_bit)<<"\n";
    std::cout<<std::format("DEST code: {:#X}", metadata.dest_code)<<"\n";
    std::cout<<std::format("VERSION: {:#X}", metadata.version)<<"\n";
}

bool Cartridge::init(const char* file) {
    bool read_file_success = read_rom(file);
    save_name = get_save_file_name(file);
    if (!read_file_success) return false;
    RomMetadata metadata = read_rom_metadata();
    if (metadata.cgb_flag == 0x80 || metadata.cgb_flag == 0xC0) cgb_mode = true;
    else cgb_mode = false;
    mbc_type = metadata.mbc_type;
    bool read_boot_success = load_boot(cgb_mode);
    if (!read_boot_success) return false;

    external_ram_size = metadata.max_ram_banks?0x2000*metadata.max_ram_banks:1;

    if (0x1 <= metadata.mbc_type && metadata.mbc_type <= 0x3) {
        mbc = new MBC1(metadata.max_rom_banks, metadata.max_rom_bank_bit, metadata.max_ram_banks, metadata.max_ram_bank_bit);
    }
    else if (0x0F <= metadata.mbc_type && metadata.mbc_type <= 0x13) {
        mbc = new MBC3(this, metadata.max_rom_banks, metadata.max_rom_bank_bit, metadata.max_ram_banks, metadata.max_ram_bank_bit);
        external_ram_size += 14;
    }
    else if (0x19 <= metadata.mbc_type && metadata.mbc_type <= 0x1E) {
        mbc = new MBC5(metadata.max_rom_banks, metadata.max_rom_bank_bit, metadata.max_ram_banks, metadata.max_ram_bank_bit);
    }

    if (metadata.mbc_type != 0 && mbc == nullptr) {
        std::cerr<<"Unsupported MBC type\n";
        return false;
    }

    external_ram = new uint8_t[external_ram_size]();
    load_save();
    return true;
}

uint8_t Cartridge::read(uint16_t addr) {
    if (is_boot && (addr < 0x100 || (cgb_mode && 0x200<=addr && addr<=0x8FF))) return boot_data[addr];
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
    std::cout<<"Trying to read VRAM from cartridge. This should not happen\n";
    return 0xFF;
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
    std::cout<<"Trying to write VRAM from cartridge. This should not happen\n";
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

