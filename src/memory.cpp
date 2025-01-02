#include "memory.h"

#include <debugger.h>
#include <format>
#include <interrupts.h>

uint8_t Memory::read(uint16_t addr) {
    if (addr == 0xFF55) {
        logger.get_logger()->debug("Reading HDMA5 register: {:#X}, HDMA current status: {:X}", (!hdma_requested & 0x1) << 7 | (unsafe_read(addr) & 0x7F), hdma_requested);
        return (!hdma_requested & 0x1) << 7 | (unsafe_read(addr) & 0x7F);
    }
    if (can_read(addr)) {
        return unsafe_read(addr);
    }
    return 0xFF;
}

void Memory::write(uint16_t addr, uint8_t data) {
    // DIV should reset if written to!
    // Cannot put this in unsafe_write because that's used to increment DIV in Timer
    // Put this in unsafe_write will write 0 everytime timer want to be incremented
    if (addr == 0xFF04) {
        unsafe_write(0xFF04, 0);
        return;
    }
    if (addr == 0xFF50) {  // Write to this turn off boot
        cartridge.boot_off();
    }
    if (addr == 0xFF46 && !dma_requested) { // Capture DMA
        Scheduler::schedule(DMA_TRANSFER, 0);
    }
    if (addr == 0xFF55) {
        if (hdma_requested == false) {
            hdma_requested = true;
            hdma_type = (data >> 7) & 0x1;
            logger.get_logger()->debug("Requesting HDMA type {:X} with length of {:#X}", hdma_type, data & 0x7F);
        }
        else {
            uint8_t terminate_bit = (data >> 7) & 0x1;
            if (terminate_bit == 0) hdma_requested = false;
            logger.get_logger()->debug("HDMA overwritten with data {:#X}, terminating bit is {:X}", data, terminate_bit);
        }
    }
    if (addr == 0xFF45) { // LYC
        uint8_t prev_stat = unsafe_read(0xFF41);
        uint8_t lyc_eq = data == unsafe_read(0xFF44);
        uint8_t new_stat = (prev_stat & 0xFB) | (lyc_eq << 2);
        write(0xFF41, new_stat);
    }
    if (addr == 0xFF41) { // Capture STAT change
        uint8_t prev_stat = unsafe_read(0xFF41);
        uint8_t changes = prev_stat ^ data;
        unsafe_write(addr, data);
        uint8_t mode = data & 0x3;
        uint8_t lyc_eq = data >> 2 & 0x1;
        for(uint8_t i = 0; i < 7; i++) {
            uint8_t is_bit_changed = (changes >> i) & 0x1;
            if (!is_bit_changed) continue;
            if (i == 2 || i == 6) {
                if ((data >> 6 & 0x1) == 1 && lyc_eq == 1) Interrupts::set_interrupt_flag(1);
            }
            else {
                // if (((data >> (mode+3)) & 0x1) == 1) Interrupts::set_interrupt_flag(1); // WILL SOMEHOW CAUSE CATASTROPHIC ERROR
                if ((data >> 3 & 0x1) == 1 && mode == 0) Interrupts::set_interrupt_flag(1);
                else if ((data >> 4 & 0x1) == 1 && mode == 1) Interrupts::set_interrupt_flag(1);
                else if ((data >> 5 & 0x1) == 1 && mode == 2) Interrupts::set_interrupt_flag(1);
            }
        }
        return;
    }
    if (addr == 0xFF68) { // Capture BGPI change event
        bg_auto_inc = data >> 7;
        uint8_t palette_addr = data & 0x3F;
        unsafe_write(0xFF69, read_bg_cram(palette_addr));
    }
    if (addr == 0xFF69) { // Capture BCPD change event
        uint8_t bgpi = unsafe_read(0xFF68);
        uint8_t palette_addr = bgpi & 0x3F;
        write_bg_cram(palette_addr, data);
        if (bg_auto_inc) write(0xFF68, (bgpi & 0xC0) | ((palette_addr + 1) % 64));
    }
    if (addr == 0xFF6A) {
        obj_auto_inc = data >> 7;
        uint8_t palette_addr = data & 0x3F;
        unsafe_write(0xFF6B, read_obj_cram(palette_addr));
    }
    if (addr == 0xFF6B) {
        uint8_t obpi = unsafe_read(0xFF6A);
        uint8_t palette_addr = obpi & 0x3F;
        write_obj_cram(palette_addr, data);
        if (obj_auto_inc) write(0xFF6A, (obpi & 0xC0) | ((palette_addr + 1) % 64));
    }
    if (addr == 0xFF14) {
        c1_trigger = data >> 7;
    }
    if (addr == 0xFF19) {
        c2_trigger = data >> 7;
    }
    if (addr == 0xFF1E) {
        c3_trigger = data >> 7;
    }
    if (addr == 0xFF4F) {
        vram_bank = data & 0x1;
    }
    if (addr == 0xFF70) {
        uint8_t bank_number = data & 0x7;
        wram_bank = bank_number?bank_number-1:0;
    }
    if (can_write(addr)) {
        unsafe_write(addr, data);
    }
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
    if (addr <= 0x7FFF || (0xA000 <= addr && addr <= 0xBFFF)) {
        return cartridge.read(addr);
    }
    if (0xD000 <= addr && addr <= 0xDFFF) {
        return read_wram(addr, wram_bank);
    }
    if (0x8000 <= addr && addr <= 0x9FFF) {
        return read_vram(addr, vram_bank);
    }
    if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
        return memory[addr - 0xC000 - 0x2000];
    }
    return memory[addr - 0xC000];
}

void Memory::unsafe_write(uint16_t addr, uint8_t data) {
    // Re-route
    if (addr <= 0x7FFF || (0xA000 <= addr && addr <= 0xBFFF)) {
        return cartridge.write(addr, data);
    }
    if (0xD000 <= addr && addr <= 0xDFFF) {
        return write_wram(addr, data, wram_bank);
    }
    if (0x8000 <= addr && addr <= 0x9FFF) {
        return write_vram(addr, data, vram_bank);
    }
    if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
        *(memory+addr - 0xC000 - 0x2000) = data;
        return;
    }

    *(memory+addr - 0xC000) = data;
}

bool Memory::init_cartridge(const char* file) {
    return cartridge.init(file);
}

bool Memory::is_cartridge_cgb() {
    return cartridge.cgb_mode;
}

void Memory::close_cartridge() {
    cartridge.close();
}

uint8_t Memory::read_vram(uint16_t addr, uint8_t bank) { // Low level VRAM access - Won't automatically use current bank
    uint16_t real_addr = addr - 0x8000;
    return vram[real_addr + 0x2000 * bank];
}

void Memory::write_vram(uint16_t addr, uint8_t data, uint8_t bank) { // Low level VRAM access - Won't automatically use current bank
    // logger.get_logger()->debug("Writing to VRAM at addr: {:X}, translated to: {:X}", addr, addr - 0x8000);
    uint16_t real_addr = addr - 0x8000;
    vram[real_addr + 0x2000 * bank] = data;
}

uint8_t Memory::read_wram(uint16_t addr, uint8_t bank) { // Low level WRAM access - Won't automatically use current bank
    uint16_t real_addr = addr - 0xD000;
    return wram[real_addr + 0x1000 * bank];
}

void Memory::write_wram(uint16_t addr, uint8_t data, uint8_t bank) { // Low level WRAM access - Won't automatically use current bank
    // logger.get_logger()->debug("Writing to VRAM at addr: {:X}, translated to: {:X}", addr, addr - 0x8000);
    uint16_t real_addr = addr - 0xD000;
    wram[real_addr + 0x1000 * bank] = data;
}

uint8_t Memory::read_bg_cram(uint8_t addr) {
    return bg_cram[addr];
}

void Memory::write_bg_cram(uint8_t addr, uint8_t data) {
    bg_cram[addr] = data;
}

uint8_t Memory::read_obj_cram(uint8_t addr) {
    return obj_cram[addr];
}

void Memory::write_obj_cram(uint8_t addr, uint8_t data) {
    obj_cram[addr] = data;
}

bool Memory::check_hdma() {
    return hdma_requested;
}

uint8_t Memory::get_hdma_type() {
    return hdma_type;
}

void Memory::resolve_hdma() {
    hdma_requested = false;
}

uint8_t *Memory::get_raw() {
    return memory;
}

uint8_t *Memory::get_raw_vram() {
    return vram;
}

uint8_t *Memory::get_raw_wram() {
    return wram;
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