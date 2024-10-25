#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>
#include "cartridge.h"

class Memory{
private:
    static bool read_allow;
    static bool write_allow;
    static u_char* memory;
public:
    static u_char read(u_short addr);
    static void write(u_short addr, u_char data);
    static u_char unsafe_read(u_short addr);
    static void unsafe_write(u_short addr, u_char data);
    static u_char* get_raw();
    static bool can_read(u_short addr);
    static bool can_write(u_short addr);
};

#endif
