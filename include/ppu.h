#ifndef PPU_H
#define PPU_H
#include <types.h>

class Ppu {
private:
    inline static u_char *frame_buffer = new u_char[160*144]();
    u_char mode;
    u_short wait;
    u_short dots;
    u_short m3_penalties = 0;
    u_short frame_buf_index = 0;
    u_short obj_queue[10];
    u_char obj_queue_idx = 0;

    u_char enable;
    u_short w_tilemap_area;
    u_char w_enable;
    u_short bg_tilemap_area;
    u_short tiledata_area;
    u_char obj_enable;
    u_char obj_size;
    u_char bg_w_priority;

    u_char scx_lower;
    u_char scy_lower;

    u_char w_internal_lc = 0;
    u_char line_did_enable_w = 0;

    void render_background();
    void render_window();
    void render_object();
    u_short get_tile_index_from_pixel(u_char x, u_char y);
    u_char parse_palette(u_char src_color, u_short palette_addr);

public:
    void init();
    static u_char* get_frame_buffer();
    void tick();
    u_char read_ly();
    void inc_ly();
    void inc_w_internal_lc();
    u_char read_scy();
    u_char read_scx();
    u_char read_wy();
    u_char read_wx();
    void read_lcdc();
    void update_stat();
    void check_stat_interrupt();
};

#endif //PPU_H
