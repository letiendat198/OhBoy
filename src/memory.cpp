#include "memory.h"

#include <debugger.h>
#include <format>

uint8_t Memory::read(uint16_t addr) {
    if (can_read(addr)) {
        return unsafe_read(addr);
    }
    // Debugger::log(std::format("READ PROHIBITED AT ADDRESS {:#X}", addr));
    return 0xFF;
}

void Memory::write(uint16_t addr, uint8_t data) {
    // DIV should reset if written to!
    // Cannot put this in unsafe_write because that's used to increment DIV in Timer
    // Put this in unsafe_write will write 0 everytime timer want to be incremented
    if (addr == 0xFF04) {
        *(memory+addr - 0xC000) = 0;
        return;
    }
    if (can_write(addr)) {
        unsafe_write(addr, data);
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
    if (addr == 0xFF50) {  // Write to this turn off boot
        Cartridge::boot_off();
    }
    if (addr == 0xFF46 && !dma_requested) { // Capture DMA
        dma_requested = true;
    }
    if (addr == 0xFF41) { // Capture STAT change
        uint8_t prev_stat = unsafe_read(0xFF41);
        uint8_t changes = prev_stat ^ data;
        *(memory+addr - 0xC000) = data;
        for(uint8_t i = 0; i < 7; i++) {
            uint8_t is_bit_changed = (changes >> i) & 0x1;
            if (!is_bit_changed) continue;
            if (i == 2 || i == 6) {
                PPU::check_and_req_lyc_stat();
            }
            else {
                PPU::check_and_req_mode0_stat();
                PPU::check_and_req_mode1_stat();
                PPU::check_and_req_mode2_stat();
            }
        }
        return;
    }

    // Re-route
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
    oam_lock = true;
}

void Memory::unlock_oam() {
    oam_lock = false;
}

void Memory::lock_vram() {
    vram_lock = true;
}

void Memory::unlock_vram() {
    vram_lock = false;
}

void Memory::lock_dma() {
    dma_lock = true;
}

void Memory::unlock_dma() {
    dma_lock = false;
}