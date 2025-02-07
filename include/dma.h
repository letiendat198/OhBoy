#ifndef DMA_H
#define DMA_H
#include <memory.h>

namespace DMA {
    inline uint8_t dma_addr = 0;
    inline Logger logger = Logger("DMA");

    void transfer_dma();
};

namespace HDMA {
    inline uint16_t hdma_src = 0;
    inline uint16_t hdma_dest = 0;
    inline uint8_t hdma_length = 0;
    inline uint8_t hdma_type = 0;
    inline bool is_hdma_running = false;
    inline Logger logger = Logger("HDMA");

    void transfer_gdma();
    void transfer_hdma();
};

#endif //DMA_H
