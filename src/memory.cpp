#include "memory.h"

#include <debugger.h>
#include <format>

u_char Memory::read(u_short addr) {
    if (addr == 0xFF00) return 0xFF;
    if (can_read(addr)) {
        if (addr < 0xC000) {
            return Cartridge::read(addr);
        }
        return memory[addr - 0xC000];
    }
    else {
        // Debugger::log(std::format("READ PROHIBITED AT ADDRESS {:#X}", addr));
        return 0xFF;
    }
}

void Memory::write(u_short addr, u_char data) {
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
        *(memory+addr - 0xC000) = data;
    }
}

bool Memory::check_dma() {
    return dma_requested;
}

void Memory::resolve_dma() {
    dma_requested = false;
}


bool Memory::can_write(u_short addr) {
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

u_char Memory::unsafe_read(u_short addr) {
    if (addr < 0xC000) {
        return Cartridge::read(addr);
    }
    return memory[addr - 0xC000];
}

void Memory::unsafe_write(u_short addr, u_char data) {
    if (addr < 0xC000) {
        return Cartridge::write(addr, data);
    }
    *(memory+addr - 0xC000) = data;
}

u_char *Memory::get_raw() {
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