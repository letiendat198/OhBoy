#include "cartridge.h"
#include <filesystem>

std::string get_save_file_path(std::string file_path) {
    std::size_t ext_delim = file_path.find_last_of('.');
    file_path = file_path.substr(0, ext_delim) + ".sav";
    return file_path;
}

bool read_file(const char *file, uint8_t **output, uint32_t *size) {
    FILE *f = fopen(file, "rb");
    if (f == nullptr) {
        std::cerr << "File not found or invalid!\n";
        return false;
    }
    fseek(f, 0L, SEEK_END);
    if (*size == 0) *size = ftell(f);
    if (*output == nullptr) *output = new uint8_t[*size];
    fseek(f, 0L, SEEK_SET);
    fread(*output, sizeof(uint8_t),*size, f);
    fclose(f);
    return true;
}

bool write_file(const char *file, uint8_t *data, uint32_t size) {
    FILE *f = fopen(file, "wb");
    if (f == nullptr) {
        std::cerr << "Something went wrong while opening save file\n";
        return false;
    }
    uint32_t bytes_written = fwrite(data, sizeof(uint8_t), size, f);
    if (bytes_written != size) {
        std::cerr << "Written less than input!\n";
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

CartridgeHeader read_cartridge_header(uint8_t *rom_data) {
    CartridgeHeader metadata{};
    metadata.rom_title = new char[0xF + 1]; // Stub
    strncpy(metadata.rom_title, reinterpret_cast<const char *>(rom_data+0x0134), 16);

    metadata.mbc_type = rom_data[0x0147];
    metadata.rom_size = (32 * (1<<rom_data[0x0148]));
    metadata.rom_banks = metadata.rom_size / 16;
    uint8_t ram_bank_lookup[6] = {0, 0, 1, 4, 16, 8};
    metadata.ram_banks = ram_bank_lookup[rom_data[0x0149]];
    metadata.dest_code = rom_data[0x014A];
    metadata.version = rom_data[0x014D];
    metadata.cgb_flag = rom_data[0x143];

    return metadata;
}

bool Cartridge::init(const char *file) {
    uint32_t rom_size = 0;
    bool rom_result = read_file(file, &rom_data, &rom_size);
    if (!rom_result) return false;

    header = read_cartridge_header(rom_data);

    logger.get_logger()->debug("MBC type: {:#X}", header.mbc_type);
    logger.get_logger()->debug("ROM banks: {:d}", header.rom_banks);
    logger.get_logger()->debug("RAM banks: {:d}", header.ram_banks);

    if (header.cgb_flag == 0x80 || header.cgb_flag == 0xC0) is_cgb = true;
    else is_cgb = false;

    std::string boot_file = is_cgb ? "cgb_boot.bin" : "boot.bin";
    uint32_t boot_size = 0;
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

    external_ram_size = 0x2000*header.ram_banks;
    external_ram = new uint8_t[external_ram_size]();

    if (header.ram_banks == 0) return true;

    save_file_path = get_save_file_path(file);
    bool is_save_exist = read_file(save_file_path.c_str(), &external_ram, &external_ram_size);
    if (!is_save_exist) std::cout << "No save file found for rom "<<file<<"\n";
    return true;
}

bool Cartridge::save_sram() {
    if (header.ram_banks == 0) {
        std::cout<<"Game don't have SRAM to save\n";
        return false;
    }
    return write_file(save_file_path.c_str(), external_ram, external_ram_size);
}

