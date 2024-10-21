#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <cpu.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool is_cpu_paused=true;
    int breakpoint=0;
    ImVector<char*> serial_output;
    static ImVector<const char*> debug_console;
public:
    Cpu cpu;
    bool done;
    void init();
    void render();
    void render_registers();
    void render_memdump();
    void tick_cpu();
    void end();

    static void log(const char* c);
};

#endif //DEBUGGER_H
