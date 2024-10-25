#include "ppu.h"

#include <debugger.h>
#include <format>
#include <memory.h>

#include "interrupts.h"

void Ppu::init() {
    wait = 0;
    dots = 0;
    read_lcdc();
}

void Ppu::tick() {
    if (dots++==456) { // Start of frame
        dots = 0;
        // Debugger::log(std::format("Dot reseted while fb index was {}", frame_buf_index));
        inc_ly();
        update_stat();
        check_stat_interrupt();
        if (line_did_enable_w) w_internal_lc++;
        line_did_enable_w = 0;
    }
    if(wait>0) {
        wait--;
        return;
    }
    if (dots <= 80) mode = 2;
    else if (dots <= 252+m3_penalties) mode = 3;
    else if (dots <= 456) mode = 0;

    if (read_ly() >= 144) mode = 1;

    update_stat();

    switch (mode) {
        case 0:
            break;
        case 1:
            w_internal_lc = 0;
            if (read_ly() == 144 && dots == 0) Interrupts::set_if(0);
            break;
        case 2:
            break;
        case 3:
            read_lcdc();
            if (dots <= 92) return; // Need this to sync frame_buf_index with line, since once dots reset, a line should be drawn.
            if (dots == 93) { // Mid-frame scroll behavior. Fetch 3 low bits of scx and scy
                scx_lower = read_scx() & 0x7;
                scy_lower = read_scy() & 0x7;
            }
            render_background();
            render_window();
            frame_buf_index++;
            break;
    }
    if (frame_buf_index == 160 * 144) {
        frame_buf_index = 0;
    }
}

void Ppu::render_background() {
    if (bg_w_priority == 0) {
        frame_buffer[frame_buf_index] = 0;
        return;
    }
    u_char scx = read_scx() & ~0x7 | scx_lower; // Mid-frame scroll behavior
    u_char scy = read_scy() & ~0x7 | scy_lower;
    u_char x = (scx + frame_buf_index % 160) % 256;
    u_char y = (scy + frame_buf_index / 160) % 256;

    u_short map_addr = 0x9800;
    if (bg_tilemap_area == 1) map_addr = 0x9C00;
    // Debugger::log(std::format("Reading tile map data from addr: {:X}", map_addr + get_tile_index_from_pixel(x, y)));
    u_short tile_ref = Memory::read(map_addr + get_tile_index_from_pixel(x, y));
    // Debugger::log(std::format("Tile map is referencing to tile: {}", tile_ref));
    // Debugger::log(std::format("Tile data area bit: {:X}", tiledata_area));
    u_short tile_line_data;
    u_short tile_data_ptr;
    if (tile_ref >= 128) tile_data_ptr = 0x8800;
    else {
        if (tiledata_area == 1) tile_data_ptr = 0x8000;
        else tile_data_ptr = 0x9000;
    }
    tile_ref = (tile_ref % 128)*16;
    u_short line_offset = (y % 8)*2;

    tile_line_data = Memory::read(tile_data_ptr + tile_ref + line_offset);
    tile_line_data |= Memory::read(tile_data_ptr + tile_ref + line_offset + 1) << 8;

    // Debugger::log(std::format("Fetched tile data: {:X}", tile_line_data));
    u_char pixel_offset = 7 - (x % 8);
    u_char p1 = tile_line_data & 0xFF;
    u_char p2 = tile_line_data >> 8;
    // Debugger::log(std::format("Fetching data for line with byte 1: {}, byte 2: {}", p1, p2));
    // std::cout<<(int) p1<< " " << (int) p2 <<"\n";
    frame_buffer[frame_buf_index] = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
    // Debugger::log(std::format("Color for pixel {} of line {} is {}", x, y, frame_buffer[m3_x-1]));
    // std::cout<<(int) frame_buffer[m3_x-1]<<"\n";
}

void Ppu::render_window() {
    if (bg_w_priority == 0 || w_enable == 0) {
        return;
    }
    u_char wx = read_wx();
    u_char wy = read_wy();
    if ((wx < 0) || (wx > 166)) return;
    if ((wy < 0) || (wy > 143)) return;
    u_char x = frame_buf_index % 160;
    u_char y = frame_buf_index / 160;
    if ((wx - 7) > x || wy > y) return;
    line_did_enable_w = 1;
    x = x - (wx - 7);
    y = w_internal_lc;
    // Debugger::log(std::format("Window tiles {}", get_tile_index_from_pixel(x, y)));

    u_short map_addr = 0x9800;
    if (w_tilemap_area) map_addr = 0x9C00;
    // Debugger::log(std::format("Reading tile map data from addr: {:X}", map_addr + get_tile_index_from_pixel(x, y)));
    u_short tile_ref = Memory::read(map_addr + get_tile_index_from_pixel(x, y));
    // Debugger::log(std::format("Tile map is referencing to tile: {}", tile_ref));
    // Debugger::log(std::format("Tile data area bit: {:X}", tiledata_area));
    u_short tile_line_data;
    u_short tile_data_ptr;
    if (tile_ref >= 128) tile_data_ptr = 0x8800;
    else {
        if (tiledata_area) tile_data_ptr = 0x8000;
        else tile_data_ptr = 0x9000;
    }
    tile_ref = (tile_ref % 128)*16;
    u_short line_offset = (y % 8)*2;

    tile_line_data = Memory::read(tile_data_ptr + tile_ref + line_offset);
    tile_line_data |= Memory::read(tile_data_ptr + tile_ref + line_offset + 1) << 8;

    // Debugger::log(std::format("Fetched tile data: {:X}", tile_line_data));
    u_char pixel_offset = 7 - (x % 8);
    u_char p1 = tile_line_data & 0xFF;
    u_char p2 = tile_line_data >> 8;
    // Debugger::log(std::format("Fetching data for line with byte 1: {}, byte 2: {}", p1, p2));
    // std::cout<<(int) p1<< " " << (int) p2 <<"\n";
    frame_buffer[frame_buf_index] = ((p1 >> pixel_offset) & 0x1) | (((p2 >> pixel_offset) & 0x1) << 1);
    // frame_buffer[frame_buf_index] = 0xee;
    // Debugger::log(std::format("Color for pixel {} of line {} is {}", x, y, frame_buffer[m3_x-1]));
    // std::cout<<(int) frame_buffer[m3_x-1]<<"\n";
}


u_short Ppu::get_tile_index_from_pixel(u_char x, u_char y) {
    return ((x / 8) + 32 * (y/8));
}


void Ppu::read_lcdc() {
    u_char data = Memory::read(0xFF40);
    enable = (data & 0x80) >> 7;
    w_tilemap_area = (data & 0x40) >> 6;
    w_enable = (data & 0x20) >> 5;
    tiledata_area = (data & 0x10) >> 4;
    bg_tilemap_area = (data & 0x8) >> 3;
    obj_size = (data & 0x4) >> 2;
    obj_enable = (data & 0x2) >> 1;
    bg_w_priority = (data & 0x1);
}

void Ppu::check_stat_interrupt() {
    u_char stat = Memory::read(0xFF41);
    for(int i=3; i<=6; i++) {
        u_char flag = (stat >> i) & 0x1;
        if (flag == 1) {
            switch (i) {
                case 3:
                    if (mode==0 && dots == 252+m3_penalties+1) {
                        Interrupts::set_if(1);
                    }
                    break;
                case 4:
                    if (mode==1 && dots==0 && read_ly()==144) {
                        Interrupts::set_if(1);
                    }
                    break;
                case 5:
                    if (mode==2 && dots==0 && read_ly()<144) {
                        Interrupts::set_if(1);
                    }
                    break;
                case 6:
                    if (((stat >> 2) & 0x1) == 1) {
                        Interrupts::set_if(1);
                    }
                    break;
            }
        }
    }
}


void Ppu::update_stat() {
    u_char current_stat = Memory::read(0xFF41);
    u_char lyc = Memory::read(0xFF45);
    u_char write_data = (lyc == read_ly()) << 2 |  mode;
    // Debugger::log(std::format("Current STAT: {:b}", current_stat));
    // Debugger::log(std::format("STAT update data: {:b}", write_data));
    current_stat = (current_stat & ~0x7) | write_data;
    // Debugger::log(std::format("Updated STAT: {:b}", current_stat));
    Memory::write(0xFF41, current_stat);
}


u_char Ppu::read_ly() {
    return Memory::read(0xFF44);
}

void Ppu::inc_ly() {
    // Debugger::log(std::format("LY increased to {}", read_ly()+1));
    Memory::write(0xFF44, (read_ly() + 1)  % 154);
}

// void Ppu::inc_w_internal_lc() {
//     w_internal_lc++;
//     if (read_ly() == 0) w_internal_lc = 0;
// }

u_char Ppu::read_scx() {
    return Memory::read(0xFF43);
}

u_char Ppu::read_scy() {
    return Memory::read(0xFF42);
}

u_char Ppu::read_wx() {
    return Memory::read(0xFF4B);
}

u_char Ppu::read_wy() {
    return Memory::read(0xFF4A);
}


u_char *Ppu::get_frame_buffer() {
    return frame_buffer;
}

