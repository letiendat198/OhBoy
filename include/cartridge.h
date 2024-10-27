#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <types.h>
#include <iostream>

class Cartridge{
private:
    inline static FILE *f;
    inline static FILE *f_boot;
    inline static u_char *rom_data;
    inline static u_char *cartridge_mem;
    inline static u_char *boot_data;
    inline static u_char *external_ram;
    inline static bool is_boot = true;
    inline static int rom_file_size;
    inline static int boot_size;
    inline static u_char mbc_type;
    inline static char *rom_title;
    inline static u_short rom_size;
    inline static u_short ram_size;
    inline static u_char dest_code;
    inline static u_char version;

    inline static bool ram_enable = false;
    inline static u_char rom_bank = 1;
    inline static u_char ram_bank = 0;
    inline static u_char bank_mode = 0;

    inline static u_char rom_bank_offset = 0;

public:
    static bool init(const char* file);
    static u_char read(u_short addr);
    static void write(u_short addr, u_char data);
    static void switch_rom_bank(u_char rom_bank);
    static void boot_off();
    static void close();
};

#endif
