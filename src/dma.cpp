#include "dma.h"

#include <debugger.h>

void DMA::transfer_dma() {
    dma_addr = Memory::read(0xFF46);
    uint16_t dest = 0xFE00;
    uint16_t src = dma_addr * 0x100;

    for (uint8_t cycle = 0; cycle < 160; cycle++) Memory::unsafe_write(dest + cycle, Memory::unsafe_read(src + cycle));
    Memory::unsafe_read(0xFE00 + 159);
}

void HDMA::transfer_gdma() {
    uint8_t src1 = Memory::read(0xFF51);
    uint8_t src2 = Memory::read(0xFF52);
    uint16_t src_addr = ((src1 << 8) | src2) & 0xFFF0;

    uint8_t dest1 = Memory::read(0xFF53);
    uint8_t dest2 = Memory::read(0xFF54);
    uint16_t dest_addr = 0x8000 + ((dest1 << 8 | dest2) & 0x1FF0);

    uint8_t length = Memory::unsafe_read(0xFF55) & 0x7F;

    for (int cycle = 0; cycle<(length+1) * 0x10; cycle++) {
        Memory::unsafe_write(dest_addr + cycle, Memory::unsafe_read(src_addr + cycle));
    }

    Memory::resolve_hdma();
    Memory::unsafe_write(0xFF55, 0xFF);
}


void HDMA::transfer_hdma() {
    uint8_t src1 = Memory::read(0xFF51);
    uint8_t src2 = Memory::read(0xFF52);
    uint16_t src_addr = ((src1 << 8) | src2) & 0xFFF0;

    uint8_t dest1 = Memory::read(0xFF53);
    uint8_t dest2 = Memory::read(0xFF54);
    uint16_t dest_addr = 0x8000 + ((dest1 << 8 | dest2) & 0x1FF0);

    uint8_t ppu_mode = Memory::read(0xFF41) & 0x3;
    if (ppu_mode != 0) logger.get_logger()->warn("HDMA called outside of HBlank");

     for(int i=cycle;i<cycle+16;i++) {
         Memory::unsafe_write(dest_addr + i, Memory::unsafe_read(src_addr + i));
     }

    cycle += 16;

    uint8_t new_progress = (Memory::unsafe_read(0xFF55) & 0x7F) - 1;
    // logger.get_logger()->debug("Remaining progress: {:#X}", new_progress);
    if (new_progress == 0xFF) {
        Memory::unsafe_write(0xFF55, (1<<7) | (new_progress & 0x7F));
        cycle = 0;
        Memory::resolve_hdma();
    }
    else Memory::unsafe_write(0xFF55, 0 << 7 | (new_progress & 0x7F));
}

void HDMA::reset_hdma() {
    cycle = 0;
}



