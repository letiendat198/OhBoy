#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <logger.h>
#include <types.h>

class PPU {
private:
    inline static uint8_t *frame_buffer = new uint8_t[160*144*3]();
    uint8_t dmg_palette[4][3] = {{224, 248, 208}, {136, 192, 112}, {52, 104, 86}, {8, 24, 32}};
    bool cgb_mode = false;
    uint8_t mode;
    uint16_t wait;
    uint16_t dots = 0;
    uint16_t m3_penalties = 0;
    uint16_t frame_buf_index = 0;
    uint16_t obj_queue[10];
    uint8_t obj_queue_idx = 0;

    uint8_t bg_color;

    uint8_t enable;
    uint8_t prev_enale;
    uint16_t w_tilemap_area;
    uint8_t w_enable;
    uint16_t bg_tilemap_area;
    uint16_t tiledata_area;
    uint8_t obj_enable;
    uint8_t obj_size;
    uint8_t bg_w_priority;

    uint8_t cgb_bg_priority;

    uint8_t scx_lower;
    uint8_t scy_lower;

    uint8_t w_internal_lc = 0;
    uint8_t line_did_enable_w = 0;

    Logger logger = Logger("PPU");

    void render_background();
    void render_window();
    void render_object();
    static bool compare_sprite_priority(uint16_t obj_a, uint16_t obj_b);
    uint16_t get_tile_index_from_pixel(uint8_t x, uint8_t y);
    uint8_t parse_palette(uint8_t src_color, uint16_t palette_addr);
    void write_frame_buffer(uint8_t color_id, uint8_t color_palette = 0, bool is_obj = false);

public:
    void init(bool cgb_mode);
    static uint8_t* get_frame_buffer();
    void tick();
    uint8_t read_ly();
    void write_ly(uint8_t data);
    void inc_ly();
    void inc_w_internal_lc();
    uint8_t read_scy();
    uint8_t read_scx();
    uint8_t read_wy();
    uint8_t read_wx();
    void read_lcdc();
    void update_stat();

    void update_stat_no_trigger();

    static void check_and_req_lyc_stat();
    static void check_and_req_mode0_stat();
    static void check_and_req_mode1_stat();
    static void check_and_req_mode2_stat();
};

#endif //PPU_H
