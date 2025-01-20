#include "memory.h"

#include <debugger.h>
#include <dma.h>
#include <interrupts.h>
#include <joypad.h>

uint8_t Memory::read(uint16_t addr) {
    switch (addr) {
        case 0xFFFF: { // IE
            return Interrupts::IE;
        }
        case 0xFF0F: { // IF
            return Interrupts::IF;
        }
        case 0xFF00: { // Joypad
            return Joypad::read();
        }
        case 0xFF04: { // DIV
            return (Timer::calc_current_div() >> 6) & 0xFF;
        }
        case 0xFF05: { // TIMA
            logger.get_logger()->debug("Read TIMA: {:d} at cycle: {:d}, DIV: {:b}", Timer::calc_current_tima(), Scheduler::current_cycle, Timer::calc_current_div());
            if (Timer::current_tac.enable) return Timer::calc_current_tima();
            else return Timer::paused_tima;
        }
        case 0xFF06: { // TMA
            return Timer::tma;
        }
        case 0xFF55: {
            //logger.get_logger()->debug("Reading HDMA register: {:#X}, HDMA current status: {:X}", (!hdma_requested & 0x1) << 7 | (unsafe_read(addr) & 0x7F), hdma_requested);
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
        case 0xFFFF: { // IE
            Interrupts::IE = data;
            return;
        }
        case 0xFF0F: { // IF
            Interrupts::IF = data;
            return;
        }
        case 0xFF00: { // Joypad
            Joypad::select(data >> 4 & 0x3);
            return;
        }
        case 0xFF04: // DIV RESET
        {
            logger.get_logger()->debug("DIV reset at cycle: {:d}", Scheduler::current_cycle);
            Scheduler::remove_schedule(DIV_OVERFLOW);
            Timer::schedule_next_div_overflow();
            // If DIV reset and TIMA currently enable, reschedule to match. DON'T SCHEDULE WHEN OFF - KIRBY 2 DMG CARE FOR THIS
            // Also should not reschedule TIMA every DIV overflow, or it will be push back into oblivion!
            if (Timer::current_tac.enable) Timer::schedule_tima_overflow(Timer::calc_current_tima());
            return;
        }
        case 0xFF05: { //TIMA
            if (Timer::current_tac.enable) Timer::schedule_tima_overflow(data);
            else Timer::paused_tima = data;
            logger.get_logger()->debug("Write: {:d} to TIMA at cycle: {:d}. Overflow at: {:d}. DIV: {:b}", data, Scheduler::current_cycle, Timer::tima_overflow_cycle,  Timer::calc_current_div());
            return;
        }
        case 0xFF07: { // TAC
            TimerControl tac = Timer::read_tac(data);
            TimerControl old_tac = Timer::current_tac;
            Timer::current_tac = tac;
            uint16_t div = Timer::calc_current_div();

            if (tac.enable && !old_tac.enable) Timer::schedule_tima_overflow(Timer::paused_tima); // On enable
            else if (!tac.enable && old_tac.enable) { // On disable
                // Save current TIMA
                Timer::paused_tima = Timer::calc_current_tima();
                Scheduler::remove_schedule(TIMA_OVERFLOW);
                // If TIMA disable when DIV selected bit is high, increase TIMA
                if (((div>>tac.bit_select) & 0x1) == 1) {
                    Timer::paused_tima = (Timer::paused_tima + 1) & 0xFF;
                    if (Timer::paused_tima == 0) {
                        Interrupts::set_interrupt_flag(2);
                        Timer::paused_tima = Timer::tma;
                    }
                }
            }
            // logger.get_logger()->debug("TAC enable: {}, TAC delay: {:d}", tac.enable, tac.increment_freq);
            if (tac.enable && tac.increment_freq != old_tac.increment_freq) { // On freq change - Potentially problematic
                Timer::schedule_tima_overflow(Timer::calc_current_tima());
                if (((div>>old_tac.bit_select) & 0x1) == 1 && ((div>>tac.bit_select) & 0x1) == 0) Timer::tick_tima_once();
            }
            break;
        }
        case 0xFF06: { // TMA
            Timer::tma = data;
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
                // logger.get_logger()->debug("Requesting HDMA type {:X} with length of {:#X}", hdma_type, data & 0x7F);
                if (hdma_type == 0) Scheduler::schedule(GDMA_TRANSFER, 0);
            }
            else {
                uint8_t terminate_bit = (data >> 7) & 0x1;
                if (terminate_bit == 0) {
                    hdma_requested = false;
                    HDMA::reset_hdma();
                }
                // logger.get_logger()->debug("HDMA overwritten with data {:#X}, terminating bit is {:X}", data, terminate_bit);
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
    switch((addr & 0xF000) >> 12) {
        case 0:
        case 1:
        case 2:
        case 3:
            if (is_boot && (addr < 0x100 || (cartridge.is_cgb && 0x200<=addr && addr<=0x8FF))) return *(cartridge.boot_data + addr);
            return *(cartridge.rom_data + addr);
        case 4:
        case 5:
        case 6:
        case 7:
            return *(cartridge.rom_data + (addr - 0x4000) + cartridge.rom_bank*0x4000);
        case 8:
        case 9:
            return read_vram(addr, vram_bank);
        case 0xA:
        case 0xB:
            if (cartridge.ram_enable) return *(cartridge.external_ram + (addr - 0xA000) + cartridge.ram_bank*0x2000);
            else return 0xFF;
        case 0xC:
            return wram[addr - 0xC000];
        case 0xD:
            return wram[addr - 0xD000 + 0x1000 * wram_bank];
        default:
            return memory[addr - 0xE000];
    }
}

void Memory::unsafe_write(uint16_t addr, uint8_t data) {
    // Re-route
    switch ((addr & 0xF000) >> 12) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            cartridge.mbc.update_registers(addr, data);
            break;
        case 8:
        case 9:
            write_vram(addr, data, vram_bank);
            break;
        case 0xA:
        case 0xB:
            if (cartridge.ram_enable) *(cartridge.external_ram + (addr - 0xA000) + cartridge.ram_bank*0x2000) = data;
            break;
        case 0xC:
            wram[addr - 0xC000] = data;
            break;
        case 0xD:
            wram[addr - 0xD000 + 0x1000 * wram_bank] = data;
            break;
        default:
            *(memory+addr - 0xE000) = data;
    }
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