#ifndef DMA_H
#define DMA_H
#include <memory.h>

namespace DMA {
    inline uint8_t dma_addr;
    inline uint8_t cycle = 0;
    inline Logger logger = Logger("DMA");

    void transfer_dma();
};

namespace HDMA {
    inline uint16_t cycle = 0;
    inline Logger logger = Logger("HDMA");

    void transfer_hdma();
    void reset_hdma();
    void transfer_gdma();
};

#endif //DMA_H
