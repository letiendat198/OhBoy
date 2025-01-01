#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <logger.h>

#include "mbc.h"

struct RomMetadata {
    uint8_t mbc_type;
    char *rom_title;
    uint16_t rom_size;
    uint16_t max_rom_banks;
    uint16_t max_rom_bank_bit = 0;
    uint16_t ram_size;
    uint16_t max_ram_bank_bit;
    uint8_t max_ram_banks;
    uint8_t dest_code;
    uint8_t version;
    uint8_t cgb_flag;
};

class Cartridge{
private:
    Logger logger = Logger("Cartridge");
    FILE *f;
    std::string save_name;
    FILE *f_boot;
    uint8_t *rom_data;
    uint8_t *boot_data;
    bool is_boot = true;
    int rom_file_size;
    int boot_size;

    MBC *mbc;

    bool read_rom(const char* file);
    RomMetadata read_rom_metadata();

    bool load_boot(bool is_cgb);
public:
    uint8_t *external_ram;
    uint32_t external_ram_size;

    bool cgb_mode = false;
    uint8_t mbc_type = 0;

    bool init(const char* file);

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void boot_off();
    void save_sram();
    void load_save();
    void close();
};

#endif
