#include "dma.h"

#include <debugger.h>

void DMA::transfer_dma() {
    uint16_t dest = 0xFE00;
    uint16_t src = dma_addr * 0x100;

    // bus->dma(dest, src, 160);
    for (uint8_t cycle = 0; cycle < 160; cycle++) bus->unsafe_write(dest + cycle, bus->unsafe_read(src + cycle));
}

void DMA::transfer_gdma() {

    // logger.get_logger()->debug("GDMA transfer from: {:#X} to: {:#X} with length: {:d}", hdma_src, 0x8000 + hdma_dest, (hdma_length+1) * 0x10);

    uint16_t length = (hdma_length+1) * 0x10;

    for (uint16_t i = 0; i<length; i++) {
        bus->unsafe_write(0x8000 + hdma_dest + i, bus->unsafe_read(hdma_src + i));
    }
    // bus->dma(0x8000 + hdma_dest, hdma_src, length);
    hdma_src += length;
    hdma_dest += length;

    is_hdma_running = false;
    hdma_length = 0xFF;
}


void DMA::transfer_hdma() { // TODO: HDMA stop prematurely if dest addr overflow (including ignored upper 3 bits)
    // logger.get_logger()->debug("HDMA transfer from: {:#X} to: {:#X}", hdma_src, 0x8000 + hdma_dest);

     for(uint8_t i=0;i<16;i++) {
         bus->unsafe_write(0x8000 + hdma_dest + i, bus->unsafe_read(hdma_src + i));
     }

    // bus->dma(0x8000 + hdma_dest, hdma_src, 0x10);

    hdma_src += 16;
    hdma_dest += 16;

    hdma_length -= 1;
    // logger.get_logger()->debug("Remaining progress: {:#X}", new_progress);
    if (hdma_length == 0xFF) {
        is_hdma_running = false;
    }
}