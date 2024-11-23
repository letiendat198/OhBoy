#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>
#include "cartridge.h"

class Memory{
private:
    inline static uint8_t* memory = (unsigned char*)malloc((0x3FFF+1)*sizeof(char));
    inline static bool oam_lock = false;
    inline static bool vram_lock = false;
    inline static bool dma_lock = false;
    inline static bool dma_requested = false;
    inline static Logger logger = Logger("Memory");
public:
    static uint8_t read(uint16_t addr);
    static void write(uint16_t addr, uint8_t data);
    static uint8_t unsafe_read(uint16_t addr);
    static void unsafe_write(uint16_t addr, uint8_t data);
    static uint8_t* get_raw();
    static bool can_read(uint16_t addr);
    static bool can_write(uint16_t addr);
    static void lock_oam();
    static void unlock_oam();
    static void lock_vram();
    static void unlock_vram();
    static void lock_dma();
    static void unlock_dma();
    static void turnoff_boot();
    static bool check_dma();
    static void resolve_dma();
};

#endif
