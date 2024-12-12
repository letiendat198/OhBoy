#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <apu.h>
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

void audio_callback(void *userdata, Uint8 *stream, int len);

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    inline static SDL_AudioDeviceID audioDeviceID;
    bool is_cpu_paused=false;
    int breakpoint=0;
    std::string serial_output;
    inline static std::deque<std::string> debug_buffer;
    inline static MemoryEditor memory_editor;
    SDL_Texture* used_textures[127] = {nullptr};
    SDL_Texture* old_game_texture = nullptr;

    bool is_debug = false;
public:
    inline static Logger logger = Logger("Debugger");
    Timer timer;
    CPU cpu;
    PPU ppu;
    APU apu;
    bool done;
    void init(bool debug);
    void render();
    void render_console(const ImGuiIO& io);
    void render_registers();
    void render_game();
    void queue_audio();
    void tick_cpu();
    static void capture_keyboard();
    void end();

    static void log(std::string);
};

#endif //DEBUGGER_H
