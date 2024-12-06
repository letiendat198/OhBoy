#include "dma.h"

#include <debugger.h>

void DMA::tick() {
    if (cycle == 0) {
        bool dma_requested = Memory::check_dma();
        if (!dma_requested) return;
        dma_addr = Memory::read(0xFF46);
        // Memory::lock_dma(); // DMA locking causes a lot of glitch. Disable for now
    }
    uint16_t dest = 0xFE00;
    uint16_t src = dma_addr*0x100;
    Memory::unsafe_write(dest+cycle, Memory::unsafe_read(src+cycle));
    cycle++;
    if(cycle == 160) {
        cycle = 0;
        Memory::resolve_dma();
        Memory::unlock_dma();
    }
}

void HDMA::tick() {
    if (cycle == 0) {
        bool hdma_requested = Memory::check_hdma();
        if (!hdma_requested) return;
        hdma_type = Memory::get_hdma_type();
        // logger.get_logger()->debug("HDMA started, type: {:X}, length: {:#X}", hdma_type, Memory::unsafe_read(0xFF55) & 0x7F);
    }

    if (Memory::check_hdma() == false) {
        // logger.get_logger()->debug("HDMA stopped prematurely");
        cycle = 0;
        hdma_hblank_limit = false;
        return;
    }

    uint8_t src1 = Memory::read(0xFF51);
    uint8_t src2 = Memory::read(0xFF52);
    uint16_t src_addr = ((src1 << 8) | src2) & 0xFFF0;

    uint8_t dest1 = Memory::read(0xFF53);
    uint8_t dest2 = Memory::read(0xFF54);
    uint16_t dest_addr = 0x8000 + ((dest1 << 8 | dest2) & 0x1FF0);

    uint8_t ppu_mode = Memory::read(0xFF41) & 0x3;

    if (hdma_type == 0 || (hdma_type == 1 && ppu_mode == 0)) {
        if (hdma_type == 1 && hdma_hblank_limit == true) return;
        Memory::unsafe_write(dest_addr + cycle*2, Memory::unsafe_read(src_addr + cycle*2));
        Memory::unsafe_write(dest_addr + cycle*2 + 1, Memory::unsafe_read(src_addr + cycle*2 + 1));
        // logger.get_logger()->debug("HDMA write through, source: {:#X} - dest: {:#X}", src_addr + cycle*2, dest_addr + cycle*2);
        // logger.get_logger()->debug("Assert data in source: {:#X} = dest: {:#X}", Memory::unsafe_read(src_addr + cycle*2), Memory::unsafe_read(dest_addr + cycle*2));
        cycle++;
    }
    else {
        // logger.get_logger()->debug("HDMA type 1 waiting for HBlank");
        hdma_hblank_limit = false;
        return; // Return if waiting for HBLANK
    }

    // Only update progress when cycle has transfer 16 blocks - 8 M-Cycle
    if (cycle % 8 == 0) {
        uint8_t new_progress = (Memory::unsafe_read(0xFF55) & 0x7F) - 1;
        // logger.get_logger()->debug("Remaining progress: {:#X}", new_progress);
        if (new_progress == 0xFF) {
            Memory::unsafe_write(0xFF55, (1<<7) | (new_progress & 0x7F));
            Memory::resolve_hdma();
            hdma_hblank_limit = false;
            cycle = 0;
        }
        else {
            Memory::unsafe_write(0xFF55, 0 << 7 | (new_progress & 0x7F));
        }
        if (hdma_type == 1) hdma_hblank_limit = true;
    }
}


