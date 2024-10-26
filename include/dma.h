#ifndef DMA_H
#define DMA_H
#include <memory.h>
#include <types.h>

class Dma {
private:
    inline static u_char dma_addr;
    inline static u_char cycle = 0;
public:
    static void tick();
};

#endif //DMA_H
