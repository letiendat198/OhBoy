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

#include "scheduler.h"
#include "timer.h"

void audio_callback(void *userdata, Uint8 *stream, int len);

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioDeviceID audioDeviceID;

    MemoryEditor memory_editor;
    SDL_Texture* old_game_texture = nullptr;

    bool is_debug = false;
    uint8_t scale = 3;

    Scheduler *scheduler = nullptr;
public:
    inline static Logger logger = Logger("Debugger");
    bool done = false;
    double last_frame_duration = 0;
    int breakpoint = 0;

    Debugger(Scheduler *scheduler, bool debug);
    void render();
    void render_registers(const ImGuiIO& io);
    void render_game();
    static void capture_keyboard();
    void end();
};

#endif //DEBUGGER_H
