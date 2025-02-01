#ifndef DMA_H
#define DMA_H
#include <memory.h>

namespace DMA {
    inline uint8_t dma_addr = 0;
    inline Logger logger = Logger("DMA");

    void transfer_dma();
};

namespace HDMA {
    inline uint8_t hdma_start_addr1 = 0;
    inline uint8_t hdma_start_addr2 = 0;
    inline uint8_t hdma_end_addr1 = 0;
    inline uint8_t hdma_end_addr2 = 0;
    inline uint8_t hdma_length = 0;
    inline uint8_t hdma_type = 0;
    inline bool is_hdma_running = false;
    inline uint16_t cycle = 0;
    inline Logger logger = Logger("HDMA");

    void transfer_gdma();
    void transfer_hdma();
    void reset_hdma();
};

#endif //DMA_H
