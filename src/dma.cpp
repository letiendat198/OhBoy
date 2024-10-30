#include "dma.h"

#include <debugger.h>
#include <format>

void DMA::tick() {
    if (cycle == 0) {
        bool dma_requested = Memory::check_dma();
        if (!dma_requested) return;
        dma_addr = Memory::read(0xFF46);
        // Debugger::log("DMA started");
        // Memory::lock_dma(); // DMA locking causes a lot of glitch. Disable for now
    }
    uint16_t dest = 0xFE00;
    uint16_t src = dma_addr*0x100;
    // Debugger::log(std::format("DMA writing from {:#X} to {:#X}: {:#X}", src+cycle, dest + cycle, Memory::unsafe_read(src+cycle)));
    Memory::unsafe_write(dest+cycle, Memory::unsafe_read(src+cycle));
    cycle++;
    if(cycle == 160) {
        // Debugger::log("DMA ended");
        cycle = 0;
        Memory::resolve_dma();
        Memory::unlock_dma();
    }
}

