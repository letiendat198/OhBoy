#include "cartridge.h"
#include <filesystem>

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

void Cartridge::init_boot(uint8_t *data, bool is_cgb) {
    boot_data = data;
    is_boot_cgb = is_cgb;
}

bool Cartridge::init_rom(uint8_t *data, uint32_t rom_size) {
    rom_data = data;

    header = read_cartridge_header(rom_data);

    SPDLOG_LOGGER_DEBUG(logger.get_logger(), "MBC type: {:#X}", header.mbc_type);
    SPDLOG_LOGGER_DEBUG(logger.get_logger(), "ROM banks: {:d}", header.rom_banks);
    SPDLOG_LOGGER_DEBUG(logger.get_logger(), "RAM banks: {:d}", header.ram_banks);

    if (header.cgb_flag == 0x80 || header.cgb_flag == 0xC0) is_boot_cgb = true;
    else is_boot_cgb = false;

    if (header.mbc_type == 0x0) mbc.mbc_type = MBC0;
    else if (0x1 <= header.mbc_type && header.mbc_type <= 0x3) mbc.mbc_type = MBC1;
    else if (0xF <= header.mbc_type && header.mbc_type <= 0x13) mbc.mbc_type = MBC3;
    else if (0x19 <= header.mbc_type && header.mbc_type <= 0x1E) mbc.mbc_type = MBC5;
    else {
        std::cerr << "Unsupported MBC type!\n";
        return false;
    }

    external_ram_size = 0x2000*header.ram_banks;
#ifndef ESP_PLATFORM
    external_ram = new uint8_t[external_ram_size]();
#endif
    return true;
}