#ifndef MEMORY_H
#define MEMORY_H

#include "cartridge.h"

namespace Memory{
    inline uint8_t* memory = new uint8_t[0x2000]();
    inline uint8_t* vram = new uint8_t[0x2000*2]();
    inline uint8_t* wram = new uint8_t[0x1000*8]();

    inline uint8_t* bg_cram = new uint8_t[64]();
    inline uint8_t* obj_cram = new uint8_t[64]();
    inline bool bg_auto_inc = false;
    inline bool obj_auto_inc = false;

    inline uint8_t vram_bank = 0;
    inline uint8_t wram_bank = 1;

    inline bool is_boot = true;

    inline Logger logger = Logger("Memory");

    inline Cartridge cartridge;

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    uint8_t read_vram(uint16_t addr, uint8_t bank);
    void write_vram(uint16_t addr, uint8_t data, uint8_t bank);
    uint8_t read_bg_cram(uint8_t addr);
    void write_bg_cram(uint8_t addr, uint8_t data);
    uint8_t read_obj_cram(uint8_t addr);
    void write_obj_cram(uint8_t addr, uint8_t data);
    uint8_t unsafe_read(uint16_t addr);
    void unsafe_write(uint16_t addr, uint8_t data);

    void memcpy(uint16_t dest_addr, uint16_t src_addr, uint16_t length);
};

#endif
