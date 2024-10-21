#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <types.h>
#include <iostream>

class Cartridge{
private:
    static FILE *f;
    static int size;
public:
    static bool init(const char* file);
    static void read_to_mem();
    static void write_to_sram(u_short addr);
    static void close();
};

#endif
