#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>
#include "cartridge.h"

class Memory{
private:
    inline static u_char* memory = (unsigned char*)malloc((0xFFFF+1)*sizeof(char));
    inline static bool oam_lock = false;
    inline static bool vram_lock = false;
    inline static bool dma_lock = false;
public:
    static u_char read(u_short addr);
    static void write(u_short addr, u_char data);
    static u_char unsafe_read(u_short addr);
    static void unsafe_write(u_short addr, u_char data);
    static u_char* get_raw();
    static bool can_read(u_short addr);
    static bool can_write(u_short addr);
    static void lock_oam();
    static void unlock_oam();
    static void lock_vram();
    static void unlock_vram();
    static void lock_dma();
    static void unlock_dma();
};

#endif
