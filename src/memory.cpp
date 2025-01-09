#include "memory.h"

#include <debugger.h>
#include <dma.h>
#include <format>
#include <interrupts.h>

uint8_t Memory::read(uint16_t addr) {
    switch (addr) {
        case 0xFF55: {
            logger.get_logger()->debug("Reading HDMA5 register: {:#X}, HDMA current status: {:X}", (!hdma_requested & 0x1) << 7 | (unsafe_read(addr) & 0x7F), hdma_requested);
            return (!hdma_requested & 0x1) << 7 | (unsafe_read(addr) & 0x7F);
        }
    }
    return unsafe_read(addr);
}

void Memory::write(uint16_t addr, uint8_t data) {
    // DIV should reset if written to!
    // Cannot put this in unsafe_write because that's used to increment DIV in Timer
    // Put this in unsafe_write will write 0 everytime timer want to be incremented
    switch (addr) {
        case 0xFF04: // DIV
        {
            unsafe_write(0xFF04, 0);
            return;
        }
        case 0xFF50:  // Write to this turn off boot
        {
            is_boot = false;
            break;
        }
        case 0xFF46: // Capture DMA
        {
            Scheduler::schedule(DMA_TRANSFER, 0);
            break;
        }
        case 0xFF55: {
            if (hdma_requested == false) {
                hdma_requested = true;
                hdma_type = (data >> 7) & 0x1;
                logger.get_logger()->debug("Requesting HDMA type {:X} with length of {:#X}", hdma_type, data & 0x7F);
                if (hdma_type == 0) Scheduler::schedule(GDMA_TRANSFER, 0);
            }
            else {
                uint8_t terminate_bit = (data >> 7) & 0x1;
                if (terminate_bit == 0) {
                    hdma_requested = false;
                    HDMA::reset_hdma();
                }
                logger.get_logger()->debug("HDMA overwritten with data {:#X}, terminating bit is {:X}", data, terminate_bit);
            }
            break;
        }
        case 0xFF45: // LYC
        {
            uint8_t stat = unsafe_read(0xFF41);
            uint8_t lyc_eq = data == unsafe_read(0xFF44);
            uint8_t new_stat = (stat & 0xFB) | (lyc_eq << 2);
            write(0xFF41, new_stat);
            break;
        }
        case 0xFF41: // Capture STAT change
        {
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
        case 0xFF68: // Capture BGPI change event
        {
            bg_auto_inc = data >> 7;
            uint8_t palette_addr = data & 0x3F;
            unsafe_write(0xFF69, read_bg_cram(palette_addr));
            break;
        }
        case 0xFF69: // Capture BCPD change event
        {
            uint8_t bgpi = unsafe_read(0xFF68);
            uint8_t palette_addr = bgpi & 0x3F;
            write_bg_cram(palette_addr, data);
            if (bg_auto_inc) write(0xFF68, (bgpi & 0xC0) | ((palette_addr + 1) % 64));
            break;
        }
        case 0xFF6A: {
            obj_auto_inc = data >> 7;
            uint8_t palette_addr = data & 0x3F;
            unsafe_write(0xFF6B, read_obj_cram(palette_addr));
            break;
        }
        case 0xFF6B: {
            uint8_t obpi = unsafe_read(0xFF6A);
            uint8_t palette_addr = obpi & 0x3F;
            write_obj_cram(palette_addr, data);
            if (obj_auto_inc) write(0xFF6A, (obpi & 0xC0) | ((palette_addr + 1) % 64));
            break;
        }
        case 0xFF4F: {
            vram_bank = data & 0x1;
            break;
        }
        case 0xFF70: {
            uint8_t bank_number = data & 0x7;
            wram_bank = bank_number?bank_number:1;
            break;
        }
    }

    unsafe_write(addr, data);
}

uint8_t Memory::unsafe_read(uint16_t addr) {
    if (addr <= 0x3FFF) {
        if (is_boot && (addr < 0x100 || (cartridge.is_cgb && 0x200<=addr && addr<=0x8FF))) return *(cartridge.boot_data + addr);
        return *(cartridge.rom_data + addr);
    }
    if (0x4000 <= addr && addr <= 0x7FFF) {
        return *(cartridge.rom_data + (addr - 0x4000) + cartridge.rom_bank*0x4000);
    }
    if (0xA000 <= addr && addr <= 0xBFFF) {
        if (cartridge.ram_enable) return *(cartridge.external_ram + (addr - 0xA000) + cartridge.ram_bank*0x2000);
        else return 0xFF;
    }
    if (0xC000 <= addr && addr <= 0xCFFF) {
        return read_wram(addr, 0);
    }
    if (0xD000 <= addr && addr <= 0xDFFF) {
        return read_wram(addr, wram_bank);
    }
    if (0x8000 <= addr && addr <= 0x9FFF) {
        return read_vram(addr, vram_bank);
    }
    return memory[addr - 0xE000];
}

void Memory::unsafe_write(uint16_t addr, uint8_t data) {
    // Re-route
    if (addr <= 0x7FFF) {
        cartridge.mbc.update_registers(addr, data);
        return;
    }
     if (0xA000 <= addr && addr <= 0xBFFF) {
        if (cartridge.ram_enable) *(cartridge.external_ram + (addr - 0xA000) + cartridge.ram_bank*0x2000) = data;
        return;
    }
    if (0xC000 <= addr && addr <= 0xCFFF) {
        return write_wram(addr, data, 0);
    }
    if (0xD000 <= addr && addr <= 0xDFFF) {
        return write_wram(addr, data, wram_bank);
    }
    if (0x8000 <= addr && addr <= 0x9FFF) {
        return write_vram(addr, data, vram_bank);
    }

    *(memory+addr - 0xE000) = data;
}

void Memory::memcpy(uint16_t dest_addr, uint16_t src_addr, uint16_t length) {
    uint16_t fragment_points[7] = {0x3FFF, 0x7FFF, 0x9FFF, 0xBFFF, 0xCFFF, 0xDFFF, 0xFFFF};
    uint8_t *fragment_ptr[7] = {cartridge.rom_data, cartridge.rom_data + cartridge.rom_bank*0x4000,
        vram + vram_bank*0x2000, cartridge.external_ram + cartridge.ram_bank*0x2000,
        wram, wram + wram_bank*0x1000, memory};

    uint8_t src_blk = 0;
    uint8_t dest_blk = 0;
    for(;src_blk<7;src_blk++) if (fragment_points[src_blk] >= src_addr) break;
    for(;dest_blk<7;dest_blk++) if (fragment_points[dest_blk] >= dest_addr) break;


}


bool Memory::init_cartridge(const char* file) {
    return cartridge.init(file);
}

bool Memory::is_cartridge_cgb() {
    return cartridge.is_cgb;
}

void Memory::close_cartridge() {
    // cartridge.close();
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
    uint16_t real_addr = addr - 0xC000;
    return wram[real_addr + 0x1000 * bank];
}

void Memory::write_wram(uint16_t addr, uint8_t data, uint8_t bank) { // Low level WRAM access - Won't automatically use current bank
    // logger.get_logger()->debug("Writing to VRAM at addr: {:X}, translated to: {:X}", addr, addr - 0x8000);
    uint16_t real_addr = addr - 0xC000;
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