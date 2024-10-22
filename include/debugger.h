#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <cpu.h>
#include <deque>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_memory_editor.h"
#include <SDL.h>
#include <iostream>
#include <vector>

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool is_cpu_paused=true;
    int breakpoint=0;
    std::string serial_output;
    inline static std::deque<std::string> debug_buffer;
    inline static MemoryEditor memory_editor;
public:
    Cpu cpu;
    bool done;
    void init();
    void render();
    void render_registers();
    void render_memdump();
    void tick_cpu();
    void end();

    static void log(std::string);
};

#endif //DEBUGGER_H
