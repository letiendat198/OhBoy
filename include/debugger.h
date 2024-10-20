//
// Created by ADMIN on 20/10/2024.
//

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
public:
    bool done;
    void init();
    void start();
    void ui();
    void render();
    void end();
};

#endif //DEBUGGER_H
