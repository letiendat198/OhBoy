#ifndef DMA_H
#define DMA_H
#include <memory.h>

class DMA {
private:
    inline static uint8_t dma_addr;
    inline static uint8_t cycle = 0;
public:
    static void tick();
};

#endif //DMA_H
