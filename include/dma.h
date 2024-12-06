#ifndef DMA_H
#define DMA_H
#include <memory.h>

class DMA {
private:
    inline static uint8_t dma_addr;
    inline static uint8_t cycle = 0;
    inline static Logger logger = Logger("DMA");
public:
    static void tick();
};

class HDMA {
private:
    inline static uint16_t cycle = 0;
    inline static uint8_t hdma_type = 0;
    inline static bool hdma_hblank_limit = false;
    inline static Logger logger = Logger("HDMA");
public:
    static void tick();
};

#endif //DMA_H
