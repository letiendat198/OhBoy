#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <logger.h>
#include <mbc.h>

struct CartridgeHeader {
    uint8_t mbc_type = 0;
    char *rom_title = nullptr;
    uint16_t rom_size = 0;
    uint16_t rom_banks = 0;
    uint8_t ram_banks = 0;
    uint8_t dest_code = 0;
    uint8_t version = 0;
    uint8_t cgb_flag = 0;
};

class Cartridge{
private:
    Logger logger = Logger("Cartridge");
public:
    CartridgeHeader header;
    uint8_t *boot_data = nullptr;
    uint8_t *rom_data = nullptr;
    uint8_t *external_ram = nullptr;
    uint32_t rom_bank = 1;
    uint32_t ram_bank = 0;
    bool ram_enable = false;

    uint32_t external_ram_size = 0;

    bool is_cgb = false;
    MBC mbc = MBC(this);

    bool init(const char *file);
};

#endif