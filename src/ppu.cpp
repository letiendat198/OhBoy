#include "ppu.h"
#include "interrupts.h"
#include <algorithm>
#include <debugger.h>
#include <memory.h>

void PPU::init() {
    wait = 0;
    dots = 0;
    Memory::unsafe_write(0xFF40, 0b10000000);
    read_lcdc();
}

void PPU::tick() {
    read_lcdc();
    if (enable == 0) {  // If LCD disabled, don't do anything but keep incrementing dots? Somehow work for Pokemon Gold
        if (prev_enale == 1) {
            // logger.get_logger()->debug("LCD turned off, resetting register");
            mode = 0;
            dots = 0;
            write_ly(0);
            update_stat_no_trigger();
        }
        prev_enale = enable;
        return;
    }
    if (prev_enale == 0) { // When LCD turned back on, reset PPU
        // logger.get_logger()->debug("LCD turned on again");
        write_ly(0);
        mode = 0;
        dots = 0;
        update_stat();
        prev_enale = enable;
    }
    if (dots==456) { // Start of line
        dots = 0;
        // Debugger::log(std::format("Dot reseted while fb index was {}", frame_buf_index));
        inc_ly();
        update_stat();
        if (line_did_enable_w) w_internal_lc++;
        line_did_enable_w = 0;
    }
    if (dots < 80) mode = 2;
    else if (dots < 252+m3_penalties) mode = 3;
    else if (dots < 456) mode = 0;

    if (read_ly() >= 144) mode = 1;

    update_stat();

    switch (mode) {
        case 0: //HBLANK
            break;
        case 1: // VBLANK
            w_internal_lc = 0;
            if (read_ly() == 144 && dots == 0) {
                Interrupts::set_interrupt_flag(0);
            }
            break;
        case 2: // OAM scan
            if (dots == 0) {
                memset(obj_queue, 0, 10);
                obj_queue_idx = 0;
            }
            if (dots % 2 == 0 && obj_queue_idx < 10) {
                uint8_t ly = read_ly();
                uint16_t obj_addr = 0xFE00 + dots * 2;
                uint8_t obj_y = Memory::unsafe_read(obj_addr);
                // if (obj_y >= 16) Debugger::log(std::format("OAM obj y: {}", obj_y));
                uint8_t size = obj_size ? 16 : 8;
                if ((obj_y - 16) <= ly && ly < (obj_y - 16 + size)) {
                    obj_queue[obj_queue_idx++] = obj_addr;
                }
            }
            if (dots == 79) {
                std::stable_sort(obj_queue, obj_queue + obj_queue_idx, compare_sprite_priority);
            }
            break;
        case 3: // TODO: Implement mode 3 penalty
            if (dots < 92) break; // Need this to sync frame_buf_index with line, since once dots reset, a line should be drawn.
            if (dots == 92) { // Mid-frame scroll behavior. Fetch 3 low bits of scx and scy
                scx_lower = read_scx() & 0x7;
                scy_lower = read_scy() & 0x7;
                // std::stable_sort(std::begin(obj_queue), std::end(obj_queue), compare);
            }
            render_background();
            render_window();
            render_object();
            frame_buf_index++;
            break;
    }
    dots++;
    if (frame_buf_index == 160 * 144) {
        frame_buf_index = 0;
    }
}

void PPU::render_background() {
    if (bg_w_priority == 0) {
        frame_buffer[frame_buf_index] = 0;
        return;
    }
    uint8_t scx = read_scx() & ~0x7 | scx_lower; // Mid-frame scroll behavior
    uint8_t scy = read_scy() & ~0x7 | scy_lower;
    uint8_t x = (scx + frame_buf_index % 160) % 256;
    uint8_t y = (scy + frame_buf_index / 160) % 256;

    uint16_t map_addr = 0x9800;
    if (bg_tilemap_area == 1) map_addr = 0x9C00;
    // Debugger::log(std::format("Reading tile map data from addr: {:X}", map_addr + get_tile_index_from_pixel(x, y)));
    uint16_t tile_ref = Memory::unsafe_read(map_addr + get_tile_index_from_pixel(x, y));
    // Debugger::log(std::format("Tile map is referencing to tile: {}", tile_ref));
    // Debugger::log(std::format("Tile data area bit: {:X}", tiledata_area));
    uint16_t tile_line_data;
    uint16_t tile_data_ptr;
    if (tile_ref >= 128) tile_data_ptr = 0x8800;
    else {
        if (tiledata_area == 1) tile_data_ptr = 0x8000;
        else tile_data_ptr = 0x9000;
    }
    tile_ref = (tile_ref % 128)*16;
    uint16_t line_offset = (y % 8)*2;

    tile_line_data = Memory::unsafe_read(tile_data_ptr + tile_ref + line_offset);
    tile_line_data |= Memory::unsafe_read(tile_data_ptr + tile_ref + line_offset + 1) << 8;

    // Debugger::log(std::format("Fetched tile data: {:X}", tile_line_data));
    uint8_t pixel_offset = 7 - (x % 8);
    uint8_t p1 = tile_line_data & 0xFF;
    uint8_t p2 = tile_line_data >> 8;
    // Debugger::log(std::format("Fetching data for line with byte 1: {}, byte 2: {}", p1, p2));
    // std::cout<<(int) p1<< " " << (int) p2 <<"\n";
    uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
    frame_buffer[frame_buf_index] = parse_palette(color, 0xFF47);
    // Debugger::log(std::format("Color for pixel {} of line {} is {}", x, y, frame_buffer[m3_x-1]));
    // std::cout<<(int) frame_buffer[m3_x-1]<<"\n";
}

void PPU::render_window() {
    if (bg_w_priority == 0 || w_enable == 0) {
        return;
    }
    uint8_t wx = read_wx();
    uint8_t wy = read_wy();
    if ((wx < 0) || (wx > 166)) return;
    if ((wy < 0) || (wy > 143)) return;
    uint8_t x = frame_buf_index % 160;
    uint8_t y = frame_buf_index / 160;
    if ((wx - 7) > x || wy > y) return;
    line_did_enable_w = 1;
    x = x - (wx - 7);
    y = w_internal_lc;
    // Debugger::log(std::format("Window tiles {}", get_tile_index_from_pixel(x, y)));

    uint16_t map_addr = 0x9800;
    if (w_tilemap_area) map_addr = 0x9C00;
    // Debugger::log(std::format("Reading tile map data from addr: {:X}", map_addr + get_tile_index_from_pixel(x, y)));
    uint16_t tile_ref = Memory::unsafe_read(map_addr + get_tile_index_from_pixel(x, y));
    // Debugger::log(std::format("Tile map is referencing to tile: {}", tile_ref));
    // Debugger::log(std::format("Tile data area bit: {:X}", tiledata_area));
    uint16_t tile_line_data;
    uint16_t tile_data_ptr;
    if (tile_ref >= 128) tile_data_ptr = 0x8800;
    else {
        if (tiledata_area) tile_data_ptr = 0x8000;
        else tile_data_ptr = 0x9000;
    }
    tile_ref = (tile_ref % 128)*16;
    uint16_t line_offset = (y % 8)*2;

    tile_line_data = Memory::unsafe_read(tile_data_ptr + tile_ref + line_offset);
    tile_line_data |= Memory::unsafe_read(tile_data_ptr + tile_ref + line_offset + 1) << 8;

    // Debugger::log(std::format("Fetched tile data: {:X}", tile_line_data));
    uint8_t pixel_offset = 7 - (x % 8);
    uint8_t p1 = tile_line_data & 0xFF;
    uint8_t p2 = tile_line_data >> 8;
    // Debugger::log(std::format("Fetching data for line with byte 1: {}, byte 2: {}", p1, p2));
    // std::cout<<(int) p1<< " " << (int) p2 <<"\n";
    uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
    frame_buffer[frame_buf_index] = parse_palette(color, 0xFF47);
    // frame_buffer[frame_buf_index] = 0xee;
    // Debugger::log(std::format("Color for pixel {} of line {} is {}", x, y, frame_buffer[m3_x-1]));
    // std::cout<<(int) frame_buffer[m3_x-1]<<"\n";
}

void PPU::render_object() {
    if (obj_enable == 0) {
        return;
    }
    uint8_t x = frame_buf_index % 160;
    uint8_t y = frame_buf_index / 160;

    for (uint8_t i=0;i<obj_queue_idx;i++) {
        uint16_t obj_addr = obj_queue[i];
        uint8_t obj_x_start = Memory::unsafe_read(obj_addr + 1);
        if (!(obj_x_start - 8 <= x  && x < obj_x_start)) continue;

        uint8_t obj_att = Memory::unsafe_read(obj_addr + 3);
        uint8_t obj_priority = (obj_att >> 7) & 0x1;
        uint8_t obj_y_flip = (obj_att >> 6) & 0x1;
        uint8_t obj_x_flip = (obj_att >> 5) & 0x1;
        uint8_t obj_palette = (obj_att >> 4) & 0x1;
        uint16_t obj_y_start = Memory::unsafe_read(obj_addr);

        if (obj_priority == 1) {
            if (frame_buffer[frame_buf_index] > 0) return; // Bg/Window color 1-3 draw over this obj
        }

        uint8_t size = obj_size ? 16 : 8;
        uint16_t tile_ref = Memory::unsafe_read(obj_addr + 2);
        if (size == 16) {
            if ((y-obj_y_start+16)%16 < 8) tile_ref = tile_ref & 0xFE;
            else tile_ref = tile_ref | 0x01;

            if (obj_y_flip == 1) tile_ref ^= 0x01;
        }

        uint16_t tile_line_data;
        uint16_t tile_data_ptr;
        if (tile_ref >= 128) tile_data_ptr = 0x8800;
        else tile_data_ptr = 0x8000;

        tile_ref = (tile_ref % 128)*16;
        uint16_t line_offset = ((y - obj_y_start + 16) % 8)*2;
        if(obj_y_flip == 1) line_offset = (7 - ((y - obj_y_start + 16) % 8))*2;

        tile_line_data = Memory::unsafe_read(tile_data_ptr + tile_ref + line_offset);
        tile_line_data |= Memory::unsafe_read(tile_data_ptr + tile_ref + line_offset + 1) << 8;

        // Because obj can be anywhere, it's pixel offset should not be calculated by x % 8 -> cause clipping if obj starting x pos not divisible by 8
        // But should be calculated by (obj starting x cord - current x) to find which pixel to render
        uint8_t pixel_offset = 7 - ((x - obj_x_start + 8) % 8);
        if (obj_x_flip == 1) pixel_offset = (x - obj_x_start + 8) % 8;
        uint8_t p1 = tile_line_data & 0xFF;
        uint8_t p2 = tile_line_data >> 8;

        uint8_t color = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);

        uint16_t palette_addr = obj_palette ? 0xFF49 : 0xFF48;
        if (color > 0) {
            frame_buffer[frame_buf_index] = parse_palette(color, palette_addr);
            break;
        }
    }
}

bool PPU::compare_sprite_priority(uint16_t obj_a, uint16_t obj_b) {
    uint8_t obj_ax = Memory::unsafe_read(obj_a + 1);
    uint8_t obj_bx = Memory::unsafe_read(obj_b + 1);
    return obj_ax < obj_bx;
}


uint16_t PPU::get_tile_index_from_pixel(uint8_t x, uint8_t y) {
    return ((x / 8) + 32 * (y/8));
}

uint8_t PPU::parse_palette(uint8_t src_color, uint16_t palette_addr) {
    uint8_t palette_data = Memory::unsafe_read(palette_addr);
    return (palette_data >> src_color * 2) & 0x3;
}

void PPU::read_lcdc() {
    uint8_t data = Memory::unsafe_read(0xFF40);
    enable = (data & 0x80) >> 7;
    w_tilemap_area = (data & 0x40) >> 6;
    w_enable = (data & 0x20) >> 5;
    tiledata_area = (data & 0x10) >> 4;
    bg_tilemap_area = (data & 0x8) >> 3;
    obj_size = (data & 0x4) >> 2;
    obj_enable = (data & 0x2) >> 1;
    bg_w_priority = (data & 0x1);
}

void PPU::check_and_req_lyc_stat() {
    uint8_t stat = Memory::unsafe_read(0xFF41);
    uint8_t flag = (stat >> 6) & 0x1;
    if (flag && ((stat >> 2) & 0x1) == 1) {
        Interrupts::set_interrupt_flag(1);
    }
}

void PPU::check_and_req_mode0_stat() {
    uint8_t stat = Memory::unsafe_read(0xFF41);
    uint8_t flag = (stat >> 3) & 0x1;
    uint8_t mode = stat & 0x3;
    if (flag && mode==0) {
        Interrupts::set_interrupt_flag(1);
    }
}

void PPU::check_and_req_mode1_stat() {
    uint8_t stat = Memory::unsafe_read(0xFF41);
    uint8_t flag = (stat >> 4) & 0x1;
    uint8_t mode = stat & 0x3;
    if (flag && mode==1) {
        Interrupts::set_interrupt_flag(1);
    }
}

void PPU::check_and_req_mode2_stat() {
    uint8_t stat = Memory::unsafe_read(0xFF41);
    uint8_t flag = (stat >> 5) & 0x1;
    uint8_t mode = stat & 0x3;
    if (flag && mode==2) {
        Interrupts::set_interrupt_flag(1);
    }
}

void PPU::update_stat() {
    uint8_t prev_stat = Memory::unsafe_read(0xFF41);
    uint8_t lyc = Memory::unsafe_read(0xFF45);
    uint8_t write_data = (lyc == read_ly()) << 2 |  mode;
    uint8_t new_stat = (prev_stat & 0xF8) | write_data;
    // spdlog::info("Current mode: {}", mode);
    // spdlog::info("Current LYC and LY: {} {}", lyc, read_ly());
    // spdlog::info("New stat: {:08b}", new_stat);
    Memory::unsafe_write(0xFF41, new_stat);
}

void PPU::update_stat_no_trigger() {
    uint8_t prev_stat = Memory::unsafe_read(0xFF41);
    uint8_t lyc = Memory::unsafe_read(0xFF45);
    uint8_t write_data = (lyc == read_ly()) << 2 |  mode;
    uint8_t new_stat = (prev_stat & 0xF8) | write_data;
    // spdlog::info("Current mode: {}", mode);
    // spdlog::info("Current LYC and LY: {} {}", lyc, read_ly());
    // spdlog::info("New stat: {:08b}", new_stat);
    Memory::get_raw()[0xFF41 - 0xC000] = new_stat;
}

uint8_t PPU::read_ly() {
    return Memory::unsafe_read(0xFF44);
}

void PPU::inc_ly() {
    // Debugger::log(std::format("LY increased to {}", read_ly()+1));
    Memory::unsafe_write(0xFF44, (read_ly() + 1)  % 154);
}

void PPU::write_ly(uint8_t data) {
    Memory::unsafe_write(0xFF44, data);
}

// void Ppu::inc_w_internal_lc() {
//     w_internal_lc++;
//     if (read_ly() == 0) w_internal_lc = 0;
// }

uint8_t PPU::read_scx() {
    return Memory::unsafe_read(0xFF43);
}

uint8_t PPU::read_scy() {
    return Memory::unsafe_read(0xFF42);
}

uint8_t PPU::read_wx() {
    return Memory::unsafe_read(0xFF4B);
}

uint8_t PPU::read_wy() {
    return Memory::unsafe_read(0xFF4A);
}

uint8_t *PPU::get_frame_buffer() {
    return frame_buffer;
}


