#include "dma.h"

#include <debugger.h>
#include <format>

void Dma::tick() {
    u_char dma = Memory::read(0xFF46);
    if (dma == prev_dma || dma==0xFF) return;

    if (cycle == 0) {
        Debugger::log(std::format("DMA switch from {:#X} to {:#X}", prev_dma, dma));
        Debugger::log("DMA started");
        Memory::lock_dma();
    }
    u_short dest = 0xFE00;
    u_short src = dma << 8 | 0x00;
    Memory::unsafe_write(dest+cycle, Memory::unsafe_read(src));
    cycle++;
    if(cycle == 160) {
        prev_dma = dma;
        cycle = 0;
        Memory::unlock_dma();
    }
}

