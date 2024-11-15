#include "memory.h"

#include <debugger.h>
#include <format>

uint8_t Memory::read(uint16_t addr) {
    if (can_read(addr)) {
        if (addr < 0xC000) {
            return Cartridge::read(addr);
        }
        if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
            return memory[addr - 0xC000 - 0x2000];
        }
        return memory[addr - 0xC000];
    }
    else {
        // Debugger::log(std::format("READ PROHIBITED AT ADDRESS {:#X}", addr));
        return 0xFF;
    }
}

void Memory::write(uint16_t addr, uint8_t data) {
    if (addr == 0xFF50) {
        Cartridge::boot_off();
    }
    if (addr == 0xFF04) {
        *(memory+addr - 0xC000) = 0;
        return;
    }
    if (addr == 0xFF46 && !dma_requested) {
        dma_requested = true;
    }
    if (can_write(addr)) {
        if (addr < 0xC000) {
            return Cartridge::write(addr, data);
        }
        if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
            *(memory+addr - 0xC000 - 0x2000) = data;
            return;
        }
        *(memory+addr - 0xC000) = data;
    }
}

bool Memory::check_dma() {
    return dma_requested;
}

void Memory::resolve_dma() {
    dma_requested = false;
}


bool Memory::can_write(uint16_t addr) {
    if ((0xE000 <= addr && addr <= 0xFDFF) || (0xFEA0 <= addr && addr <= 0xFEFF)) {
        return false;
    }
    if (oam_lock && (0xFE00 <= addr && addr <= 0xFE9F)) return false;
    if (vram_lock && (0x8000 <= addr && addr <= 0x9FFF)) return false;
    if (dma_lock && (addr < 0xFF80 || addr > 0xFFFE)) return false;
    return true;
}

bool Memory::can_read(unsigned short addr) {
    if (oam_lock && (0xFE00 <= addr && addr <= 0xFE9F)) return false;
    if (vram_lock && (0x8000 <= addr && addr <= 0x9FFF)) return false;
    if (dma_lock && (addr < 0xFF80 || addr > 0xFFFE)) return false;
    return true;
}

uint8_t Memory::unsafe_read(uint16_t addr) {
    if (addr < 0xC000) {
        return Cartridge::read(addr);
    }
    if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
        return memory[addr - 0xC000 - 0x2000];
    }
    return memory[addr - 0xC000];
}

void Memory::unsafe_write(uint16_t addr, uint8_t data) {
    if (addr < 0xC000) {
        return Cartridge::write(addr, data);
    }
    if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
        *(memory+addr - 0xC000 - 0x2000) = data;
        return;
    }
    *(memory+addr - 0xC000) = data;
}

uint8_t *Memory::get_raw() {
    return memory;
}

void Memory::lock_oam() {
    oam_lock = 1;
}

void Memory::unlock_oam() {
    oam_lock = 0;
}

void Memory::lock_vram() {
    vram_lock = 1;
}

void Memory::unlock_vram() {
    vram_lock = 0;
}

void Memory::lock_dma() {
    dma_lock = 1;
}

void Memory::unlock_dma() {
    dma_lock = 0;
}