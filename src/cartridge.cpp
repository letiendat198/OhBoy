#include "cartridge.h"
#include <filesystem>

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

bool read_file(const char *file, uint8_t **output, uint32_t *size) {
    FILE *f = fopen(file, "rb");
    if (f == nullptr) {
        std::cerr << "File not found or invalid!\n";
        output = nullptr;
        *size = 0;
        return false;
    }
    fseek(f, 0L, SEEK_END);
    *size = ftell(f);
    *output = new uint8_t[*size];
    fseek(f, 0L, SEEK_SET);
    fread(*output, sizeof(uint8_t),*size, f);
    return true;
}

CartridgeHeader read_cartridge_header(uint8_t *rom_data) {
    CartridgeHeader metadata{};
    metadata.rom_title = new char[0xF + 1]; // Stub
    strncpy(metadata.rom_title, reinterpret_cast<const char *>(rom_data+0x0134), 16);

    metadata.mbc_type = rom_data[0x0147];
    metadata.rom_size = (32 * (1<<rom_data[0x0148]));
    metadata.rom_banks = metadata.rom_size / 16;
    uint8_t ram_size = rom_data[0x0149];
    switch (ram_size) {
        case 0x0:
        case 0x1:
            metadata.ram_banks = 0;
            break;
        case 0x2:
            metadata.ram_banks = 1;
            break;
        case 0x3:
            metadata.ram_banks = 4;
            break;
        case 0x4:
            metadata.ram_banks = 16;
            break;
        case 0x5:
            metadata.ram_banks = 8;
            break;
    }
    metadata.dest_code = rom_data[0x014A];
    metadata.version = rom_data[0x014D];
    metadata.cgb_flag = rom_data[0x143];

    return metadata;
}

bool Cartridge::init(const char *file) {
    uint32_t rom_size;
    bool rom_result = read_file(file, &rom_data, &rom_size);
    if (!rom_result) return false;

    header = read_cartridge_header(rom_data);

    if (header.cgb_flag == 0x80 || header.cgb_flag == 0xC0) is_cgb = true;
    else is_cgb = false;

    std::string boot_file = is_cgb ? "cgb_boot.bin" : "boot.bin";
    uint32_t boot_size;
    bool boot_result = read_file(boot_file.c_str(), &boot_data, &boot_size);
    if (!boot_result) return false;

    if (header.mbc_type == 0x0) mbc.mbc_type = MBC0;
    else if (0x1 <= header.mbc_type && header.mbc_type <= 0x3) mbc.mbc_type = MBC1;
    else if (0xF <= header.mbc_type && header.mbc_type <= 0x13) mbc.mbc_type = MBC3;
    else if (0x19 <= header.mbc_type && header.mbc_type <= 0x1E) mbc.mbc_type = MBC5;
    else {
        std::cerr << "Unsupported MBC type!\n";
        return false;
    }

    external_ram = new uint8_t[0x2000*header.ram_banks];
    return true;
}
