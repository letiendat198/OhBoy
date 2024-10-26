#include "memory.h"

#include <debugger.h>
#include <format>

u_char Memory::read(u_short addr) {
    if (boot_off == false && addr <= 0xFF) {
        return bootrom[addr];
    }

    if (can_read(addr)) {
        return memory[addr];
    }
    else {
        // Debugger::log(std::format("READ PROHIBITED AT ADDRESS {:#X}", addr));
        return 0xFF;
    }
}

void Memory::write(u_short addr, u_char data) {
    // if (0xA000 <= addr && addr <= 0xBFFF) {
    //     return;
    // }
    if (addr == 0xFF50 && boot_off == false) {
        Debugger::log("Turn off bootrom");
        boot_off = true;
    }

    if (addr == 0xFF04) {
        *(memory+addr) = 0;
        return;
    }
    if (can_write(addr)) {
        *(memory+addr) = data;
    }
}

u_char Memory::unsafe_read(u_short addr) {
    return memory[addr];
}

void Memory::unsafe_write(u_short addr, u_char data) {
    *(memory+addr) = data;
}

u_char *Memory::get_raw() {
    return memory;
}

u_char *Memory::get_boot_raw() {
    return bootrom;
}


bool Memory::can_write(u_short addr) {
    if (addr <= 0x7FFF || (0xE000 <= addr && addr <= 0xFDFF) || (0xFEA0 <= addr && addr <= 0xFEFF)) {
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

void Memory::turnoff_boot() {
    boot_off = true;
}