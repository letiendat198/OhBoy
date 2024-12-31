#ifndef PPU_H
#define PPU_H
#include <cartridge.h>
#include <cstdint>
#include <logger.h>

struct LCDC {
    uint8_t lcd_enable;
    uint16_t window_tile_map;
    uint8_t window_enable;
    uint16_t tile_data_area;
    uint16_t bg_tile_map;
    uint8_t obj_size;
    uint8_t obj_enable;
    uint8_t bg_window_priority;
};

struct ObjAttribute {
    uint8_t y_pos;
    uint8_t x_pos;
    uint8_t tile_index;
    // Flags
    uint8_t priority;
    uint8_t y_flip;
    uint8_t x_flip;
    uint8_t dmg_palette;
    uint8_t bank;
    uint8_t cgb_palette;

    bool operator < (const ObjAttribute& a) const {
        return x_pos < a.x_pos;
    }
};

struct Scroll {
    uint8_t scx;
    uint8_t scy;
    uint8_t wx;
    uint8_t wy;
};

class PPU {
private:
    Logger logger = Logger("PPU");
    uint8_t dmg_palette[4][3] = {{224, 248, 208}, {136, 192, 112}, {52, 104, 86}, {8, 24, 32}};
    ObjAttribute obj_queue[10]{};
    uint8_t obj_queue_index = 0;
    uint8_t mode = 0;
public:
    inline static uint8_t *frame_buffer = new uint8_t[160*144*3]();
    uint16_t frame_buffer_index = 0;

    uint8_t ly = 0;
    uint8_t window_ly = 0;

    void oam_scan();

    void write_frame_buffer(uint8_t color_id, uint8_t color_palette=0x0, bool is_obj=false);

    uint8_t parse_palette(uint8_t src_color, uint16_t palette_addr);

    void draw_scanline();
    void schedule_next_mode(uint8_t current_mode);
    void update_ly();

    ObjAttribute read_obj(uint16_t addr);
    LCDC read_lcdc();
    Scroll read_scroll();
};

#endif //PPU_H
