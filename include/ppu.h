#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <types.h>

class PPU {
private:
    inline static uint8_t *frame_buffer = new uint8_t[160*144]();
    uint8_t mode;
    uint16_t wait;
    uint16_t dots;
    uint16_t m3_penalties = 0;
    uint16_t frame_buf_index = 0;
    uint16_t obj_queue[10];
    uint8_t obj_queue_idx = 0;

    uint8_t enable;
    uint16_t w_tilemap_area;
    uint8_t w_enable;
    uint16_t bg_tilemap_area;
    uint16_t tiledata_area;
    uint8_t obj_enable;
    uint8_t obj_size;
    uint8_t bg_w_priority;

    uint8_t scx_lower;
    uint8_t scy_lower;

    uint8_t w_internal_lc = 0;
    uint8_t line_did_enable_w = 0;

    void render_background();
    void render_window();
    void render_object(uint8_t offset);
    uint16_t get_tile_index_from_pixel(uint8_t x, uint8_t y);
    uint8_t parse_palette(uint8_t src_color, uint16_t palette_addr);

public:
    void init();
    static uint8_t* get_frame_buffer();
    void tick();
    uint8_t read_ly();
    void inc_ly();
    void inc_w_internal_lc();
    uint8_t read_scy();
    uint8_t read_scx();
    uint8_t read_wy();
    uint8_t read_wx();
    void read_lcdc();
    void update_stat();
    void check_and_req_lyc_stat();
    void check_and_req_mode0_stat();
    void check_and_req_mode1_stat();
    void check_and_req_mode2_stat();
};

#endif //PPU_H
