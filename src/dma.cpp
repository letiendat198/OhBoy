#include "dma.h"

#include <debugger.h>
#include <format>

void Dma::tick() {
    if (cycle == 0) {
        u_char dma = Memory::read(0xFF46);
        if (dma == prev_dma) return;
        Debugger::log(std::format("DMA switch from {:#X} to {:#X}", prev_dma, dma));
        Debugger::log("DMA started");
        Memory::lock_dma();
        prev_dma = dma;
    }
    u_short dest = 0xFE00;
    u_short src = prev_dma << 8 | 0x00;
    Memory::unsafe_write(dest+cycle, Memory::unsafe_read(src+cycle));
    cycle++;
    if(cycle == 160) {
        Debugger::log("DMA ended");
        cycle = 0;
        Memory::unlock_dma();
    }
}

