#ifndef DMA_H
#define DMA_H
#include <memory.h>

class Memory;

class DMA {
    Memory *bus = nullptr;
public:
    explicit DMA(Memory *bus): bus(bus){};
    uint8_t dma_addr = 0;
    void transfer_dma();

    uint16_t hdma_src = 0;
    uint16_t hdma_dest = 0;
    uint8_t hdma_length = 0;
    uint8_t hdma_type = 0;
    bool is_hdma_running = false;

    void transfer_gdma();
    void transfer_hdma();
};

#endif //DMA_H
