#ifndef DMA_H
#define DMA_H
#include <memory.h>

class DMA {
private:
    inline static uint8_t dma_addr;
    inline static uint8_t cycle = 0;
    inline static Logger logger = Logger("DMA");
public:
    static void transfer_dma();
};

class HDMA {
private:
    inline static uint16_t cycle = 0;
    inline static Logger logger = Logger("HDMA");
public:
    static void transfer_hdma();
    static void reset_hdma();
    static void transfer_gdma();
};

#endif //DMA_H
