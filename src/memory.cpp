#include "memory.h"

#include <debugger.h>
#include <dma.h>
#include <interrupt.h>
#include <joypad.h>

uint8_t Memory::read(uint16_t addr) {
    switch (addr) {
        case 0xFFFF: { // IE
            return Interrupt::IE;
        }
        case 0xFF0F: { // IF
            return Interrupt::IF;
        }
        case 0xFF4D: { // Double SPD mode
            return (CPU::double_spd_mode << 7) | CPU::switch_armed;
        }
        case 0xFF00: { // Joypad
            return Joypad::read();
        }
        case 0xFF04: { // DIV
            return (Timer::calc_current_div() >> 6) & 0xFF;
        }
        case 0xFF05: { // TIMA
            // logger.get_logger()->debug("Read TIMA: {:d} at cycle: {:d}, DIV: {:b}", Timer::calc_current_tima(), Scheduler::current_cycle, Timer::calc_current_div());
            if (Timer::current_tac.enable) return Timer::calc_current_tima();
            else return Timer::paused_tima;
        }
        case 0xFF06: { // TMA
            return Timer::tma;
        }
        case 0xFF46:
            return DMA::dma_addr;
        case 0xFF51: // HDMA1
        case 0xFF52: // HDMA2
        case 0xFF53: // HDMA3
        case 0xFF54: // HDMA4
            return 0xFF; // Read-only
        case 0xFF55: {
            // logger.get_logger()->debug("Reading HDMA register: {:#X}, HDMA current status: {:X}", (!HDMA::is_hdma_running) << 7 | (HDMA::hdma_length & 0x7F), HDMA::is_hdma_running);
            return (!HDMA::is_hdma_running) << 7 | (HDMA::hdma_length & 0x7F);
        }
        case 0xFF44: // LY
            return PPU::ly;
        case 0xFF45: // LYC
            return PPU::lyc;
        case 0xFF41: // STAT
            return PPU::stat_mode_selection | ((PPU::lyc == PPU::ly) << 2) | PPU::mode & 0x3;
    }
    if (0xFF10 <= addr && addr <= 0xFF2F) return scheduler->apu.read_apu_register(addr);
    return unsafe_read(addr);
}

void Memory::write(uint16_t addr, uint8_t data) {
    // DIV should reset if written to!
    // Cannot put this in unsafe_write because that's used to increment DIV in Timer
    // Put this in unsafe_write will write 0 everytime timer want to be incremented
    switch (addr) {
        case 0xFFFF: { // IE
            Interrupt::IE = data;
            return;
        }
        case 0xFF0F: { // IF
            Interrupt::IF = data;
            return;
        }
        case 0xFF00: { // Joypad
            Joypad::select(data >> 4 & 0x3);
            return;
        }
        case 0xFF4D: { // Double SPD mode
            CPU::switch_armed = data & 0x1;
            return;
        }
        case 0xFF04: { // DIV RESET
            // logger.get_logger()->debug("DIV reset at cycle: {:d}", Scheduler::current_cycle);

            // When DIV is written and causing a falling edge on selected TIMA bit, tick once
            uint16_t current_div = Timer::calc_current_div();
            if (((current_div >> Timer::current_tac.bit_select) & 0x1) == 1) Timer::tick_tima_once();

            Scheduler::remove_schedule(DIV_OVERFLOW);
            Timer::schedule_next_div_overflow();

            // If DIV reset and TIMA currently enable, reschedule to match. DON'T SCHEDULE WHEN OFF - KIRBY 2 DMG CARE FOR THIS
            // Also should not reschedule TIMA every DIV overflow, or it will be push back into oblivion!
            if (Timer::current_tac.enable) Timer::schedule_tima_overflow(Timer::calc_current_tima());
            scheduler->apu.schedule_div_apu();
            return;
        }
        case 0xFF05: { //TIMA
            if (Timer::current_tac.enable) Timer::schedule_tima_overflow(data);
            else Timer::paused_tima = data;
            // logger.get_logger()->debug("Write: {:d} to TIMA at cycle: {:d}. Overflow at: {:d}. DIV: {:b}", data, Scheduler::current_cycle, Timer::tima_overflow_cycle,  Timer::calc_current_div());
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
                        Interrupt::set_flag(TIMER_INTR);
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
            DMA::dma_addr = data;
            DMA::transfer_dma();
            // logger.get_logger()->debug("DMA initiated");
            return;
        }
        case 0xFF51: { // HDMA1
            HDMA::hdma_src &= 0x00F0;
            HDMA::hdma_src |= data << 8;
            return;
        }
        case 0xFF52: { // HDMA2
            HDMA::hdma_src &= 0xFF00;
            HDMA::hdma_src |= data & 0xF0;
            return;
        }
        case 0xFF53: { // HDMA3
            HDMA::hdma_dest &= 0x00F0;
            HDMA::hdma_dest |= (data & 0x1F) << 8;
            return;
        }
        case 0xFF54: { // HDMA4
            HDMA::hdma_dest &= 0xFF00;
            HDMA::hdma_dest |= data & 0xF0;
            return;
        }
        case 0xFF55: { // HDMA5 - Length/Mode/Start
            HDMA::hdma_length = data & 0x7F;
            if (HDMA::is_hdma_running == false) {
                HDMA::is_hdma_running = true;
                HDMA::hdma_type = (data >> 7) & 0x1;
                // logger.get_logger()->debug("Requesting HDMA type {:X} with length of {:#X}", HDMA::hdma_type, data & 0x7F);
                if (HDMA::hdma_type == 0) HDMA::transfer_gdma();
                else if (HDMA::hdma_type == 1 && PPU::mode == 0) HDMA::transfer_hdma();
            }
            else {
                uint8_t terminate_bit = (data >> 7) & 0x1;
                if (terminate_bit == 0) {
                    HDMA::is_hdma_running = false;
                }
                // logger.get_logger()->debug("HDMA overwritten with data {:#X}, terminating bit is {:X}", data, terminate_bit);
            }
            return;
        }
        case 0xFF40: { //LCDC
            uint8_t lcdc_enable = data >> 7 & 0x1;
            if (lcdc_enable && !PPU::is_enable) PPU::enable();
            else if (!lcdc_enable && PPU::is_enable) PPU::disable();
            PPU::is_enable = lcdc_enable;
            break;
        }
        case 0xFF45: // LYC
        {
            PPU::lyc = data;
            PPU::check_stat_interrupt();
            return;
        }
        case 0xFF41: // Capture STAT change
        {
            PPU::stat_mode_selection = data & 0xF8; // 3 lower bit is read-only
            PPU::check_stat_interrupt();
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

    if (0xFF10 <= addr && addr <= 0xFF2F) {
        scheduler->apu.write_apu_register(addr, data);
        return;
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
            // if (PPU::mode != 3)
                return read_vram(addr, vram_bank);
            // else {
            //     logger.get_logger()->warn("Reading VRAM in mode 3");
            //     return 0xFF;
            // }
        case 0xA:
        case 0xB:
            if (cartridge.rtc_access) return 0x0; // Shouldn't be accessible without RAM enable, but it's stubbed so doesn't really matter
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
            // if (PPU::mode != 3)
                write_vram(addr, data, vram_bank);
            // else logger.get_logger()->warn("Writing VRAM in mode 3 is forbidden");
            break;
        case 0xA:
        case 0xB:
            if (cartridge.rtc_access) break;
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