#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <types.h>
#include <iostream>

class Cartridge{
private:
    inline static FILE *f;
    inline static FILE *f_boot;
    inline static int size;
    inline static int boot_size;
public:
    static bool init(const char* file);
    static void read_to_mem();
    static void write_to_sram(u_short addr);
    static void close();
};

#endif
