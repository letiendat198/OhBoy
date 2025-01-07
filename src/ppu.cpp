#include <ppu.h>
#include <algorithm>

#include "scheduler.h"

uint16_t fetch_tile_data(uint8_t x, uint8_t y, uint16_t tile_map_region, uint16_t tile_data_area, uint8_t bank, bool y_flip) {
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

uint16_t fetch_obj_tile_data(ObjAttribute obj, LCDC lcdc, uint8_t ly) {
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
    if (lcdc.lcd_enable == false) {
        ly = 0;
        update_ly();
        update_stat(0); // STAT update won't work if not enable
        enable = false;
        return;
    }
    else enable = true;

    uint16_t tile_data = 0;
    BgAttribute tile_bg_attribute{};
    uint16_t tile_map_region = lcdc.bg_tile_map;

    // Render background/window
    uint8_t fill_table[160] = {};
    for(uint8_t i=0;i<160;i++) {
        uint8_t x = (scroll.scx + i) % 256;
        uint8_t y = (scroll.scy + ly) % 256;

        if (!is_cgb && lcdc.bg_window_priority == 0) { // In DMG mode, if bg lose priority -> white line
            write_frame_buffer(x, y, 0);
            fill_table[x] = 0;
            continue;
        }

        if (lcdc.window_enable && (i  >= (scroll.wx - 7) && ly >= scroll.wy)) {
            x = i - (scroll.wx - 7);
            y = window_ly;
            tile_map_region = lcdc.window_tile_map;
        }

        if (is_cgb) tile_bg_attribute = read_background_attribute(x, y, tile_map_region);

        tile_data = fetch_tile_data(x, y, tile_map_region, lcdc.tile_data_area, is_cgb?tile_bg_attribute.bank:0,
            is_cgb?tile_bg_attribute.y_flip:false);

        uint8_t pixel_offset = 7 - (x % 8);
        if (is_cgb && tile_bg_attribute.x_flip) pixel_offset = x % 8;
        uint8_t p1 = tile_data & 0xFF;
        uint8_t p2 = tile_data >> 8;

        uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
        fill_table[i] = color;
        if (is_cgb && tile_bg_attribute.priority) fill_table[i] |= 0xF0;
        if (!is_cgb) write_frame_buffer(i, ly, parse_palette(color, 0xFF47));
        else write_frame_buffer(i, ly, color, tile_bg_attribute.color_palette, false);
    }

    // Render objects
    for(uint8_t i=0;i<obj_queue_index;i++) {
        if (lcdc.obj_enable == 0) break;
        ObjAttribute obj = obj_queue[i];

        if (obj.x_pos == 0 || obj.x_pos >= 168) continue; // No need to render, object hidden

        tile_data = fetch_obj_tile_data(obj, lcdc, ly);

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
            uint16_t palette_addr = obj.dmg_palette ? 0xFF49 : 0xFF48;
            if (color > 0) {
                fill_table[x] = 0xFF; // Set to 0xFF to indicate an obj already show on this pixel
                if (!is_cgb) write_frame_buffer(x, ly, parse_palette(color, palette_addr));
                else write_frame_buffer(x, ly, color, obj.cgb_palette, true);
            }
        }
    }

    if (lcdc.window_enable && scroll.wx <= 166 && scroll.wy <= 143 && ly >= scroll.wy) window_ly++;
}

void PPU::oam_scan() {
    if (!enable) return;
    LCDC lcdc = read_lcdc();
    obj_queue_index = 0;
    for (uint16_t addr=0xFE00; addr<=0xFE9F; addr+=4) {
        if (obj_queue_index == 10) break;

        ObjAttribute obj = read_obj(addr);
        if ((obj.y_pos - 16) <= ly && ly < (obj.y_pos - 16 + lcdc.obj_size)) {
            obj_queue[obj_queue_index++] = obj;
        }
    }
    if (!is_cgb) std::stable_sort(obj_queue, obj_queue + obj_queue_index); // TODO: Change to use 0xFF6C
}

uint32_t min(uint32_t a, uint32_t b) {
    return a<b?a:b;
}

void PPU::write_frame_buffer(uint8_t x, uint8_t y, uint8_t color_id, uint8_t color_palette, bool is_obj) { // color_palette not used in DMG mode
    int index = (y * 160 + x) * 3;
    if (!is_cgb) { // SDL uses BGR
        frame_buffer[index] = dmg_palette[color_id][2];
        frame_buffer[index+1] = dmg_palette[color_id][1];
        frame_buffer[index+2] = dmg_palette[color_id][0];
    }
    else {
        uint8_t color_addr = color_palette * 8 + (color_id * 2);
        uint8_t r;
        uint8_t g;
        uint8_t b;
        if (!is_obj) {
            r = Memory::read_bg_cram(color_addr) & 0x1F;
            g = ((Memory::read_bg_cram(color_addr) >> 5) & 0x07) | (Memory::read_bg_cram(color_addr+1) & 0x03) << 3;;
            b = (Memory::read_bg_cram(color_addr+1) >> 2) & 0x1F;
        }
        else {
            r = Memory::read_obj_cram(color_addr) & 0x1F;
            g = ((Memory::read_obj_cram(color_addr) >> 5) & 0x07) | (Memory::read_obj_cram(color_addr+1) & 0x03) << 3;
            b = (Memory::read_obj_cram(color_addr+1) >> 2) & 0x1F;
        }

        uint32_t R = (r * 26 + g *  4 + b *  2);
        uint32_t G = (         g * 24 + b *  8);
        uint32_t B = (r *  6 + g *  4 + b * 22);
        R = min(960, R) >> 2;
        G = min(960, G) >> 2;
        B = min(960, B) >> 2;

        frame_buffer[index] = B & 0xFF;
        frame_buffer[index+1] = G & 0xFF;
        frame_buffer[index+2] = R & 0xFF;
    }
}

uint8_t PPU::parse_palette(uint8_t src_color, uint16_t palette_addr) {
    uint8_t palette_data = Memory::unsafe_read(palette_addr);
    return (palette_data >> src_color * 2) & 0x3;
}

void PPU::schedule_next_mode(uint8_t current_mode) {
    switch (current_mode) {
        case 0: // HBLANK
            if (ly<143) Scheduler::schedule(SchedulerEvent::OAM_SCAN, 51);
            else Scheduler::schedule(SchedulerEvent::VBLANK, 51);
            break;
        case 1: // VBLANK
            Scheduler::schedule(SchedulerEvent::OAM_SCAN, 1140);
            break;
        case 2: // OAM SCAN
            Scheduler::schedule(SchedulerEvent::DRAW, 20);
            break;
        case 3: // DRAW
            Scheduler::schedule(SchedulerEvent::HBLANK, 43);
            break;
        default: ;
    }
}

void PPU::update_stat(uint8_t mode) {
    if (!enable) return;
    uint8_t prev_stat = Memory::unsafe_read(0xFF41);
    uint8_t lyc = Memory::unsafe_read(0xFF45);
    uint8_t write_data = (lyc == ly) << 2 |  mode;
    uint8_t new_stat = (prev_stat & 0xF8) | write_data;
    // spdlog::info("Current mode: {}", mode);
    // spdlog::info("Current LYC and LY: {} {}", lyc, read_ly());
    // spdlog::info("New stat: {:08b}", new_stat);
    Memory::write(0xFF41, new_stat);
}

ObjAttribute PPU::read_obj(uint16_t addr) {
    ObjAttribute obj{};
    obj.y_pos = Memory::read(addr);
    obj.x_pos = Memory::read(addr+1);
    obj.tile_index = Memory::read(addr+2);
    uint8_t flag = Memory::read(addr+3);
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
    uint8_t reg = Memory::read(0xFF40);
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
    scroll.scy = Memory::read(0xFF42);
    scroll.scx = Memory::read(0xFF43);
    scroll.wy = Memory::read(0xFF4A);
    scroll.wx = Memory::read(0xFF4B);
    return scroll;
}

void PPU::update_ly() {
    ly = (ly + 1) % 154;
    Memory::unsafe_write(0xFF44, ly);
}