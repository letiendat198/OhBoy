#include "dma.h"

#include <debugger.h>

void DMA::transfer_dma() {
    uint16_t dest = 0xFE00;
    uint16_t src = dma_addr * 0x100;

    for (uint8_t cycle = 0; cycle < 160; cycle++) Memory::unsafe_write(dest + cycle, Memory::unsafe_read(src + cycle));
}

void HDMA::transfer_gdma() {

    logger.get_logger()->debug("GDMA transfer from: {:#X} to: {:#X} with length: {:d}", hdma_src, 0x8000 + hdma_dest, (hdma_length+1) * 0x10);

    uint8_t length = hdma_length;

    for (int cycle = 0; cycle<(length+1) * 0x10; cycle++) {
        Memory::unsafe_write(0x8000 + hdma_dest, Memory::unsafe_read(hdma_src));
        hdma_dest++;
        hdma_src++;
    }

    is_hdma_running = false;
    hdma_length = 0xFF;
}


void HDMA::transfer_hdma() {
    logger.get_logger()->debug("HDMA transfer from: {:#X} to: {:#X}", hdma_src, 0x8000 + hdma_dest);

     for(int i=cycle;i<cycle+16;i++) {
         Memory::unsafe_write(0x8000 + hdma_dest, Memory::unsafe_read(hdma_src));
         hdma_src++;
         hdma_dest++;
     }

    cycle += 16;

    hdma_length -= 1;
    // logger.get_logger()->debug("Remaining progress: {:#X}", new_progress);
    if (hdma_length == 0xFF) {
        cycle = 0;
        is_hdma_running = false;
    }
}

void HDMA::reset_hdma() {
    cycle = 0;
}



