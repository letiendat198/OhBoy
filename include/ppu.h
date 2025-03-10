#ifndef PPU_H
#define PPU_H
#include <cartridge.h>
#include <cstdint>

class Memory;

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

struct BgAttribute {
    uint8_t priority;
    uint8_t y_flip;
    uint8_t x_flip;
    uint8_t bank;
    uint8_t color_palette;
};

struct Scroll {
    uint8_t scx;
    uint8_t scy;
    uint8_t wx;
    uint8_t wy;
};

class PPU {
private:
    // uint8_t dmg_palette[4][3] = {{224, 248, 208}, {136, 192, 112}, {52, 104, 86}, {8, 24, 32}};
    uint16_t dmg_palette[4] = {0xB633, 0x8CEF, 0x638A, 0x31C4};
    uint8_t gamma_lookup[32] = {0,6,8,10,11,12,14,15,16,17,18,18,19,20,21,22,22,23,24,24,25,26,26,27,27,28,28,29,29,30,30,31};
    ObjAttribute obj_queue[10]{};
    uint8_t obj_queue_index = 0;
    bool is_cgb = false;

    void read_palette(uint16_t *palette, uint16_t palette_addr);
    void read_cgb_palette(uint16_t *palette, uint8_t color_palette, bool is_obj);
public:
    uint16_t *frame_buffer = new uint16_t[160*144]();
    uint8_t ly = 0;
    uint8_t window_ly = 0;
    bool is_enable = false;
    bool first_line = true;
    uint8_t mode = 0;
    uint8_t lyc = 0;
    uint8_t prev_stat = 0;
    uint8_t stat_mode_selection = 0;

    Memory *bus = nullptr;
    explicit PPU(Memory *bus): bus(bus){};

    void oam_scan();
    void draw_scanline();
    void set_cgb_mode(bool is_cgb);
    void schedule_next_mode(uint8_t mode);
    void enable();
    void disable();
    void check_stat_interrupt();

    ObjAttribute read_obj(uint16_t addr);

    BgAttribute read_background_attribute(uint8_t x, uint8_t y, uint16_t tile_map_region);

    LCDC read_lcdc();
    Scroll read_scroll();
};

#endif //PPU_H
