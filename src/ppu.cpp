#include <ppu.h>
#include <algorithm>
#include <cstring>
#include <interrupt.h>

#include "scheduler.h"

inline uint16_t fetch_tile_data(uint8_t x, uint8_t y, uint16_t tile_map_region, uint16_t tile_data_area, uint8_t bank, bool y_flip) {
    uint16_t tile_map_addr = tile_map_region + ((x / 8) + 32 * (y/8));
    uint8_t tile_index = Memory::read_vram(tile_map_addr, 0);
    uint16_t tile_data_region = tile_data_area;
    if (tile_index > 127) tile_data_region = 0x8800;

    uint16_t tile_data_addr = tile_data_region + ((tile_index % 128) * 16) + ((y%8)*2);
    if (y_flip) tile_data_addr = tile_data_region + ((tile_index % 128) * 16) + ((7 - y%8)*2);
    uint16_t tile_data = Memory::read_vram(tile_data_addr, bank);
    tile_data |= Memory::read_vram(tile_data_addr + 1, bank) << 8;
    return tile_data;
}

inline uint16_t fetch_obj_tile_data(ObjAttribute obj, LCDC lcdc, uint8_t ly) {
    uint8_t tile_index = obj.tile_index;
    if (lcdc.obj_size == 16) {
        if ((ly-obj.y_pos+16)%16 < 8) tile_index = tile_index & 0xFE;
        else tile_index = tile_index | 0x01;

        if (obj.y_flip == 1) tile_index ^= 0x01;
    }

    uint16_t tile_data_region = 0x8000;
    if (tile_index > 127) tile_data_region = 0x8800;

    uint16_t line_offset = ((ly - obj.y_pos + 16) % 8)*2;
    if(obj.y_flip == 1) line_offset = (7 - ((ly - obj.y_pos + 16) % 8))*2;

    uint16_t tile_data_addr = tile_data_region + ((tile_index % 128) * 16) + line_offset;
    uint16_t tile_data = Memory::read_vram(tile_data_addr, obj.bank);
    tile_data |= Memory::read_vram(tile_data_addr + 1, obj.bank) << 8;
    return tile_data;
}

void PPU::draw_scanline() {
    Scroll scroll = read_scroll();
    LCDC lcdc = read_lcdc();

    uint16_t tile_data = 0;
    BgAttribute tile_bg_attribute{};
    uint16_t tile_map_region = lcdc.bg_tile_map;

    uint8_t fill_table[160] = {};
    bool window_region = false;

    uint16_t bg_palette[4]{};
    uint16_t obj_palette0[4]{};
    uint16_t obj_palette1[4]{};

    if (!is_cgb) {
        read_palette(bg_palette, 0xFF47);
        read_palette(obj_palette0, 0xFF48);
        read_palette(obj_palette1, 0xFF49);
    }

    uint8_t frame_buffer_line = ly + ((ly / 3) * 2 + (ly % 3 != 0));

    // Render background/window
    for(uint8_t i=0;i<160;i++) {
        if (!is_cgb && lcdc.bg_window_priority == 0) { // In DMG mode, if bg lose priority -> white line
            // frame_buffer[ly*160+i] = bg_palette[0];
            fill_table[i] = 0;
            continue;
        }

        uint8_t x = !window_region ? (scroll.scx + i) % 256 : i - (scroll.wx - 7);
        uint8_t y = !window_region ? (scroll.scy + ly) % 256 : window_ly;

        // Check if should switch to rendering window
        if (!window_region && lcdc.window_enable && (i  >= (scroll.wx - 7) && ly >= scroll.wy)) {
            x = i - (scroll.wx - 7);
            y = window_ly;
            tile_map_region = lcdc.window_tile_map;
            window_region = true;

            if (i!=0 && x%8 != 0) { // Fetch window tile if window switch in the middle of a tile
                if (is_cgb) {
                    tile_bg_attribute = read_background_attribute(x, y, tile_map_region);
                    read_cgb_palette(bg_palette, tile_bg_attribute.color_palette, false);
                }
                tile_data = fetch_tile_data(x, y, tile_map_region, lcdc.tile_data_area, is_cgb?tile_bg_attribute.bank:0,
                                    is_cgb?tile_bg_attribute.y_flip:false);
            }
        }

        if (i==0 || x%8 == 0) {
            if (is_cgb) {
                tile_bg_attribute = read_background_attribute(x, y, tile_map_region);
                read_cgb_palette(bg_palette, tile_bg_attribute.color_palette, false);
            }
            tile_data = fetch_tile_data(x, y, tile_map_region, lcdc.tile_data_area, is_cgb?tile_bg_attribute.bank:0,
                                is_cgb?tile_bg_attribute.y_flip:false);
        }

        uint8_t pixel_offset = 7 - (x % 8);
        if (is_cgb && tile_bg_attribute.x_flip) pixel_offset = x % 8;
        uint8_t p1 = tile_data & 0xFF;
        uint8_t p2 = tile_data >> 8;

        uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
        fill_table[i] = color;
        if (is_cgb && tile_bg_attribute.priority) fill_table[i] |= 0xF0;
        // frame_buffer[ly*160 + i] = bg_palette[color];

        frame_buffer[frame_buffer_line*320 + i*2] = bg_palette[color];
        frame_buffer[frame_buffer_line*320 + i*2 + 1] = bg_palette[color];
    }

    // Render objects
    for(uint8_t i=0;i<obj_queue_index;i++) {
        if (lcdc.obj_enable == 0) break;
        ObjAttribute obj = obj_queue[i];

        if (obj.x_pos == 0 || obj.x_pos >= 168) continue; // No need to render, object hidden

        tile_data = fetch_obj_tile_data(obj, lcdc, ly);
        uint16_t *obj_palette = obj.dmg_palette ? obj_palette1 : obj_palette0;
        if (is_cgb) read_cgb_palette(obj_palette, obj.cgb_palette, true);

        for (int x = obj.x_pos - 8; x<obj.x_pos;x++) { // Signed is important
            if (x<0 || x >= 160) continue; // If x < 8 -> x - 8 > 160 cause wrap around so invalidate. Same for x >= 160
            if (!is_cgb && obj.priority == 1 && fill_table[x] != 0) continue;
            if (fill_table[x] == 0xFF) continue; // Current pixel already have a higher priority obj -> Skip
            if (is_cgb && lcdc.bg_window_priority && (fill_table[x] >= 0xF0 || obj.priority) && (fill_table[x] & 0xF)>0) continue;

            uint8_t pixel_offset = 7 - ((x - obj.x_pos + 8) % 8);
            if (obj.x_flip == 1) pixel_offset = (x - obj.x_pos + 8) % 8;
            uint8_t p1 = tile_data & 0xFF;
            uint8_t p2 = tile_data >> 8;

            uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
            if (color > 0) {
                fill_table[x] = 0xFF; // Set to 0xFF to indicate an obj already show on this pixel
                // frame_buffer[ly*160 + x] = obj_palette[color];

                frame_buffer[frame_buffer_line*320 + x*2] = obj_palette[color];
                frame_buffer[frame_buffer_line*320 + x*2 + 1] = obj_palette[color];
            }
        }
    }
    if (ly % 3 != 1) std::memcpy(frame_buffer + (frame_buffer_line+1)*320, frame_buffer + frame_buffer_line*320, 320*sizeof(uint16_t));
    if (window_region) window_ly++;
}

void PPU::oam_scan() {
    if (!enable) return;
    LCDC lcdc = read_lcdc();
    obj_queue_index = 0;
    for (uint16_t addr=0xFE00; addr<=0xFE9F; addr+=4) {
        if (obj_queue_index == 10) break;

        uint8_t obj_y_pos = Memory::unsafe_read(addr);
        if ((obj_y_pos - 16) <= ly && ly < (obj_y_pos - 16 + lcdc.obj_size)) {
            ObjAttribute obj = read_obj(addr);
            obj_queue[obj_queue_index++] = obj;
        }
    }
    if (!is_cgb) std::stable_sort(obj_queue, obj_queue + obj_queue_index); // TODO: Change to use 0xFF6C
}

void PPU::read_palette(uint16_t *palette, uint16_t palette_addr) {
    uint8_t palette_data = Memory::unsafe_read(palette_addr);
    for(uint8_t i=0;i<4;i++) palette[i] = dmg_palette[(palette_data >> i * 2) & 0x3];
}

void PPU::read_cgb_palette(uint16_t *palette, uint8_t color_palette, bool is_obj) {
    for (uint8_t color_id = 0; color_id < 4; color_id++) {
        uint8_t color_addr = color_palette * 8 + (color_id * 2);
        uint8_t p1 = is_obj ? Memory::read_obj_cram(color_addr) : Memory::read_bg_cram(color_addr);
        uint8_t p2 = is_obj ? Memory::read_obj_cram(color_addr + 1) : Memory::read_bg_cram(color_addr + 1);

        uint8_t r = p1 & 0x1F;
        uint8_t g = ((p1 >> 5) & 0x7) | (p2 & 0x3) << 3;
        uint8_t b = (p2 >> 2) & 0x1F;

        // Color correction. From: https://www.pokecommunity.com/threads/built-in-color-correction-for-gbc-games.448482/
        uint8_t rx = (13*r + 2*g +    b) >> 4;
        uint8_t gx = (       3*g +    b) >> 2;
        uint8_t bx = (       2*g + 14*b) >> 4;

        r = gamma_lookup[rx];
        g = gamma_lookup[gx];
        b = gamma_lookup[bx];

        palette[color_id] = (r << 11) | (g << 6) | b;
    }
}

void PPU::check_stat_interrupt() {
    uint8_t data = stat_mode_selection | (lyc==ly) << 2 | mode & 0x3;
    uint8_t changes = prev_stat ^ data;
    uint8_t lyc_eq = ly==lyc;
    for(uint8_t i = 0; i < 7; i++) {
        uint8_t is_bit_changed = (changes >> i) & 0x1;
        if (!is_bit_changed) continue;
        if (i == 2 || i == 6) {
            if ((data >> 6 & 0x1) == 1 && lyc_eq == 1) Interrupt::set_flag(STAT_INTR);
        }
        else {
            // if (((data >> (mode+3)) & 0x1) == 1) Interrupts::set_interrupt_flag(1); // WILL SOMEHOW CAUSE CATASTROPHIC ERROR
            if ((data >> 3 & 0x1) == 1 && mode == 0) Interrupt::set_flag(STAT_INTR);
            else if ((data >> 4 & 0x1) == 1 && mode == 1) Interrupt::set_flag(STAT_INTR);
            else if ((data >> 5 & 0x1) == 1 && mode == 2) Interrupt::set_flag(STAT_INTR);
        }
    }
    prev_stat = data;
}


bool is_lyc_bug_executed = false;

void PPU::schedule_next_mode(uint8_t current_mode) {
    switch (current_mode) {
        case 0: // HBLANK
            if (ly<143) Scheduler::schedule(EVENT_ID::OAM_SCAN, 51);
            else Scheduler::schedule(EVENT_ID::VBLANK, 51);
            break;
        case 1: // VBLANK
            if (ly != 0 && ly < 153) Scheduler::schedule(VBLANK, 114);
            else { // LY = 0 or 153
                if (!is_lyc_bug_executed) Scheduler::schedule(VBLANK, 1);
                else Scheduler::schedule(EVENT_ID::OAM_SCAN, 114);
                is_lyc_bug_executed = !is_lyc_bug_executed;
                first_line = true;
            }
            break;
        case 2: // OAM SCAN
            Scheduler::schedule(EVENT_ID::DRAW, 20);
            break;
        case 3: // DRAW
            Scheduler::schedule(EVENT_ID::HBLANK, 43);
            break;
        default: ;
    }
}

ObjAttribute PPU::read_obj(uint16_t addr) {
    ObjAttribute obj{};
    obj.y_pos = Memory::unsafe_read(addr);
    obj.x_pos = Memory::unsafe_read(addr+1);
    obj.tile_index = Memory::unsafe_read(addr+2);
    uint8_t flag = Memory::unsafe_read(addr+3);
    obj.priority = flag >> 7 & 0x1;
    obj.y_flip = flag >> 6 & 0x1;
    obj.x_flip = flag >> 5 & 0x1;
    obj.dmg_palette = flag >> 4 & 0x1;
    obj.bank = flag >> 3 & 0x1;
    obj.cgb_palette = flag & 0x7;
    return obj;
}

BgAttribute PPU::read_background_attribute(uint8_t x, uint8_t y, uint16_t tile_map_region) {
    uint16_t addr = tile_map_region + ((x / 8) + 32 * (y/8));
    BgAttribute att{};
    uint8_t bg_att = Memory::read_vram(addr, 1);
    att.priority = (bg_att >> 7) & 0x1;
    att.y_flip = (bg_att >> 6) & 0x1;
    att.x_flip = (bg_att >> 5) & 0x1;
    att.bank = (bg_att >> 3) & 0x1;
    att.color_palette = bg_att & 0x07;
    return att;
}

LCDC PPU::read_lcdc() {
    LCDC lcdc{};
    uint8_t reg = Memory::unsafe_read(0xFF40);
    lcdc.lcd_enable = reg >> 7 & 0x1;
    lcdc.window_tile_map = (reg >> 6 & 0x1) ? 0x9C00 : 0x9800;
    lcdc.window_enable = reg >> 5 & 0x1;
    lcdc.tile_data_area = (reg >> 4 & 0x1) ? 0x8000 : 0x9000;
    lcdc.bg_tile_map = reg >> 3 & 0x1 ? 0x9C00 : 0x9800;
    lcdc.obj_size = reg >> 2 & 0x1 ? 16 : 8;
    lcdc.obj_enable = reg >> 1 & 0x1;
    lcdc.bg_window_priority = reg & 0x1;
    return lcdc;
}

void PPU::set_cgb_mode(bool is_cgb) {
    this->is_cgb = is_cgb;
}

Scroll PPU::read_scroll() {
    Scroll scroll{};
    scroll.scy = Memory::unsafe_read(0xFF42);
    scroll.scx = Memory::unsafe_read(0xFF43);
    scroll.wy = Memory::unsafe_read(0xFF4A);
    scroll.wx = Memory::unsafe_read(0xFF4B);
    return scroll;
}

void PPU::disable() {
    ly = 0;
    mode = 0;

    Scheduler::remove_ppu_schedules();
}

void PPU::enable() {
    first_line = true;
    Scheduler::schedule(OAM_SCAN, 0);
}