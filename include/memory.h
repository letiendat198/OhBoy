#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>
#include "cartridge.h"

class Memory{
private:
    inline static uint8_t* memory = new uint8_t[0x3FFF+1]();
    inline static uint8_t* vram = new uint8_t[0x2000*2]();
    inline static uint8_t* wram = new uint8_t[0x1000*7]();
    inline static uint8_t* bg_cram = new uint8_t[64]();
    inline static uint8_t* obj_cram = new uint8_t[64]();
    inline static uint8_t vram_bank = 0;
    inline static uint8_t wram_bank = 0;
    inline static bool oam_lock = false;
    inline static bool vram_lock = false;
    inline static bool dma_lock = false;
    inline static bool dma_requested = false;
    inline static bool hdma_requested = false;
    inline static uint8_t hdma_type = 0; // 0 - GDMA, 1 - HDMA
    inline static bool bg_auto_inc = false;
    inline static bool obj_auto_inc = false;
    inline static Logger logger = Logger("Memory");
public:
    inline static bool c1_trigger = false;
    inline static bool c2_trigger = false;
    inline static bool c3_trigger = false;
    inline static bool c4_trigger = false;
    static uint8_t read(uint16_t addr);
    static void write(uint16_t addr, uint8_t data);
    static uint8_t read_vram(uint16_t addr, uint8_t bank);
    static void write_vram(uint16_t addr, uint8_t data, uint8_t bank);
    static uint8_t read_wram(uint16_t addr, uint8_t bank);
    static void write_wram(uint16_t addr, uint8_t data, uint8_t bank);
    static uint8_t read_bg_cram(uint8_t addr);
    static void write_bg_cram(uint8_t addr, uint8_t data);
    static uint8_t read_obj_cram(uint8_t addr);
    static void write_obj_cram(uint8_t addr, uint8_t data);
    static uint8_t unsafe_read(uint16_t addr);
    static void unsafe_write(uint16_t addr, uint8_t data);
    static uint8_t* get_raw();
    static uint8_t* get_raw_vram();
    static uint8_t* get_raw_wram();
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
    static bool check_hdma();
    static uint8_t get_hdma_type();
    static void resolve_hdma();
};

#endif
