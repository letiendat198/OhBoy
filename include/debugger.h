#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <cpu.h>
#include <ppu.h>
#include <deque>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_memory_editor.h"
#include <SDL.h>
#include <iostream>
#include <vector>

#include "timer.h"

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool is_cpu_paused=true;
    int breakpoint=0;
    std::string serial_output;
    inline static std::deque<std::string> debug_buffer;
    inline static MemoryEditor memory_editor;
    SDL_Color colors[4] = {{224, 248, 208, 255}, {136, 192, 112 ,255}, {52, 104, 86, 255}, {8, 24, 32, 255}};
    SDL_Texture* used_textures[127] = {nullptr};
    SDL_Texture* old_game_texture = nullptr;
public:
    Timer timer;
    Cpu cpu;
    Ppu ppu;
    bool done;
    void init();
    void render();
    void render_registers();
    void render_tiles();
    void render_game();
    void tick_cpu();
    void end();

    static void log(std::string);
};

#endif //DEBUGGER_H
