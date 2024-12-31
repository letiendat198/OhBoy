#include <ppu.h>

#include "scheduler.h"

uint16_t fetch_tile_data(uint8_t x, uint8_t y, uint16_t tile_map_region, uint16_t tile_data_area) {
    uint16_t tile_map_addr = tile_map_region + ((x / 8) + 32 * (y/8));
    uint8_t tile_index = Memory::read(tile_map_addr);
    uint16_t tile_data_region = tile_data_area;
    if (tile_index > 127) tile_data_region = 0x8800;

    uint16_t tile_data_addr = tile_data_region + ((tile_index % 128) * 16) + ((y%8)*2);
    uint16_t tile_data = Memory::read_vram(tile_data_addr, 0);
    tile_data |= Memory::read_vram(tile_data_addr + 1, 0) << 8;
    return tile_data;
}

void PPU::draw_scanline() {
    Scroll scroll = read_scroll();
    LCDC lcdc = read_lcdc();
    for(uint8_t i=0;i<160;i++) {
        uint8_t x = (scroll.scx + i) % 256;
        uint8_t y = (scroll.scy + ly) % 256;
        uint16_t tile_map_region = lcdc.bg_tile_map;
        // if (lcdc.window_enable && ly >= scroll.wy && i >= scroll.wx) {
        //     x = i - (scroll.wx - 7);
        //     y = window_ly++;
        //     tile_map_region = lcdc.window_tile_map;
        // }

        uint16_t tile_data = fetch_tile_data(x, y, tile_map_region, lcdc.tile_data_area);

        uint8_t pixel_offset = 7 - (x % 8);
        uint8_t p1 = tile_data & 0xFF;
        uint8_t p2 = tile_data >> 8;

        uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
        write_frame_buffer(parse_palette(color, 0xFF47));

        frame_buffer_index = (frame_buffer_index + 1) % (160*144);
    }
}

void PPU::oam_scan() {
    LCDC lcdc = read_lcdc();
    obj_queue_index = 0;
    for (uint16_t addr=0xFE00; addr<=0xFE9F; addr+=4) {
        if (obj_queue_index == 10) break;

        ObjAttribute obj = read_obj(addr);
        uint8_t size = lcdc.obj_size ? 16 : 8;
        if ((obj.y_pos - 16) <= ly && ly < (obj.y_pos - 16 + size)) {
            obj_queue[obj_queue_index++] = obj;
        }
    }
    std::stable_sort(obj_queue, obj_queue + obj_queue_index);
}

uint32_t min(uint32_t a, uint32_t b) {
    return a<b?a:b;
}

void PPU::write_frame_buffer(uint8_t color_id, uint8_t color_palette, bool is_obj) { // color_palette not used in DMG mode
    int index = frame_buffer_index * 3;
    if (true) { // SDL uses BGR
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

        // b = b << 3 | b >> 2;
        // r = r << 3 | r >> 2;
        // g = g << 3 | g >> 2;

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

LCDC PPU::read_lcdc() {
    LCDC lcdc{};
    uint8_t reg = Memory::read(0xFF40);
    lcdc.lcd_enable = reg >> 7 & 0x1;
    lcdc.window_tile_map = (reg >> 6 & 0x1) ? 0x9C00 : 0x9800;
    lcdc.window_enable = reg >> 5 & 0x1;
    lcdc.tile_data_area = (reg >> 4 & 0x1) ? 0x8000 : 0x9000;
    lcdc.bg_tile_map = reg >> 3 & 0x1 ? 0x9C00 : 0x9800;
    lcdc.obj_size = reg >> 2 & 0x1;
    lcdc.obj_enable = reg >> 1 & 0x1;
    lcdc.bg_window_priority = reg & 0x1;
    return lcdc;
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