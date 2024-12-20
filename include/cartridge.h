#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <logger.h>

#include "mbc.h"

class Cartridge{
private:
    inline static FILE *f;
    inline static std::string save_name;
    inline static FILE *f_boot;
    inline static uint8_t *rom_data;
    inline static uint8_t *boot_data;
    inline static bool is_boot = true;
    inline static int rom_file_size;
    inline static int boot_size;
    inline static uint8_t mbc_type;
    inline static char *rom_title;
    inline static uint16_t rom_size;
    inline static uint16_t max_rom_banks;
    inline static uint16_t max_rom_bank_bit = 0;
    inline static uint16_t ram_size;
    inline static uint16_t max_ram_bank_bit;
    inline static uint8_t max_ram_banks;
    inline static uint8_t dest_code;
    inline static uint8_t version;
    inline static uint8_t cgb_flag;

    inline static MBC *mbc;

public:
    inline static uint8_t *external_ram;
    inline static uint32_t external_ram_size;
    inline static bool cgb_mode = false;
    static bool init(const char* file);
    static uint8_t read(uint16_t addr);
    static void write(uint16_t addr, uint8_t data);
    static void mbc1_register_handler(uint16_t addr, uint8_t data);
    static void boot_off();
    static void save_sram();
    static void load_save();
    static void close();
};

#endif
