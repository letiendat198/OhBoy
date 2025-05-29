#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_memory_editor.h"
#include <SDL.h>

#include "scheduler.h"

void audio_callback(void *userdata, Uint8 *stream, int len);

class Debugger {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_AudioDeviceID audioDeviceID = 0;

    MemoryEditor memory_editor;
    SDL_Texture* old_game_texture = nullptr;

    bool hide_debug = false;
    uint8_t scale = 3;

    Scheduler *scheduler = nullptr;
public:
    inline static Logger logger = Logger("Debugger");
    bool done = false;
    double last_frame_duration = 0;
    int breakpoint = 0;

    Debugger(Scheduler *scheduler);
    void init(bool hide_debug);
    void render(uint16_t *frame_buffer);
    void render_registers(const ImGuiIO& io);
    void render_game(uint16_t *frame_buffer);
    void capture_keyboard(uint8_t *key_state);
    void queue_audio(short *sample, uint16_t sample_count);
    void end();
};

#endif //DEBUGGER_H
