#include "../include/debugger.h"

#include <cstdio>
#include <format>
#include <imgui_internal.h>

#include "dma.h"
#include "joypad.h"

void Debugger::init(bool debug) {
    is_debug = debug;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    int width = 1280;
    int height = 720;
    if (!is_debug) {
        width = 160*3 + 20;
        height = 144*3 + 35;
    }
    window = SDL_CreateWindow("OhBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return;
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    cpu.init();
    ppu.init();
}

void Debugger::tick_cpu() {
    if (!is_cpu_paused) {
        DMA::tick();
        for(int i=0;i<4;i++) ppu.tick();
        timer.tick();
        Joypad::tick();
        cpu.tick();
        uint8_t serial_data = Memory::read(0xFF01);
        if (Memory::read(0xFF02) == 0x81 && serial_data >= 32 && serial_data <= 127) {
            serial_output += serial_data;
            Memory::write(0xFF02, 0x01);
        }
    }
    if ((breakpoint != 0 && cpu.logger.line == breakpoint)) {
            is_cpu_paused = true;
    }
}

void Debugger::render() {
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    SDL_Event event;
    SDL_PollEvent(&event);
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT)
        done = true;
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        done = true;

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if(!is_debug)
        ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode, 0);
    else
        ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_AutoHideTabBar, 0);

    render_game();
    if (is_debug) {
        render_console(io);
        render_registers();
        // render_tiles();
        memory_editor.ReadOnly = true;
        memory_editor.DrawWindow("Memory Bus", Memory::get_raw(), 0xFFFF+1);
        memory_editor.DrawWindow("Frame Buffer", PPU::get_frame_buffer(), 160*144);
    }

    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, 115, 140, 153, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Debugger::render_console(const ImGuiIO& io) {
    ImGui::Begin("Debug Info");
    ImGui::Text("Frame rate:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(io.Framerate).c_str());
    if(ImGui::Button("Flush log")) {
        cpu.logger.flush();
    }
    ImGui::SeparatorText("Debug Console");
    ImGui::BeginChild("Debug Console");
    for(std::string i: debug_buffer) {
        ImGui::Text(i.c_str());
    }
    ImGui::EndChild();
    ImGui::End();
}


void Debugger::render_registers() {
    ImGui::Begin("Registers");

    if (ImGui::Button("Pause")) is_cpu_paused = true;
    ImGui::SameLine();
    if (ImGui::Button("Step")) cpu.tick();
    ImGui::SameLine();
    if (ImGui::Button("Continue")) {
        is_cpu_paused = false;
    }
    ImGui::Text("Current line:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(cpu.logger.line).c_str());
    ImGui::InputInt("Breakpoint", &breakpoint);
    if (cpu.logger.line == breakpoint) {
        ImGui::TextColored(ImVec4(255,255,0,255), "Breakpoint hit!");
    }

    ImGui::Text(std::format("A: {:02X}", cpu.a).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("B: {:02X}", cpu.b).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("C: {:02X}", cpu.c).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("D: {:02X}", cpu.d).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("E: {:02X}", cpu.e).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("H: {:02X}", cpu.h).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("L: {:02X}", cpu.l).c_str());

    ImGui::Text(std::format("PC: {:04X}", cpu.pc).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("SP: {:04X}", cpu.sp).c_str());

    ImGui::Text(std::format("Current: {:02X}", Memory::read(cpu.pc)).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("Next 1: {:02X}", Memory::read(cpu.pc+1)).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("Next 2: {:02X}", Memory::read(cpu.pc+2)).c_str());

    ImGui::SeparatorText("Flags");

    ImGui::Text(std::format("Z: {}", cpu.z_flag).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("N: {}", cpu.n_flag).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("H: {}", cpu.h_flag).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("C: {}", cpu.c_flag).c_str());

    ImGui::SeparatorText("Interrupts");

    ImGui::Text(std::format("IME: {}", cpu.ime).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("IE: {:X}", Memory::read(0xFFFF)).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("IF: {:X}", Memory::read(0xFF0F)).c_str());
    ImGui::SameLine();
    ImGui::Text(std::format("STAT: {:08b}", Memory::read(0xFF41)).c_str());

    ImGui::Text(std::format("LYC: {}", Memory::read(0xFF45)).c_str());

    ImGui::SeparatorText("Serial Output");
    ImGui::BeginChild("Serial");
        ImGui::TextWrapped(serial_output.c_str());
    ImGui::EndChild();
    ImGui::End();
}

void Debugger::render_tiles() {
    ImGui::Begin("Tiles");
    uint16_t tiles_addr = 0x8000;
    for (int i=0;i<127;i++) {
        uint8_t pix[64];
        int id = 0;
        for (int i=0; i < 15;i+=2) {
            uint8_t byte1= Memory::read(tiles_addr+i);
            uint8_t byte2 = Memory::read(tiles_addr+i+1);
            for(int j=7;j>=0;j--) {
                pix[id++] = ((byte1 >> j) & 0x1) | (((byte2 >> j) & 0x1) << 1);
            }
        }
        tiles_addr += 16;
        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)&pix, 8, 8, 8, 8, 0x0, 0x0, 0x0, 0x0);
        SDL_SetPaletteColors(surface->format->palette, colors, 0 ,4);
        if (surface == nullptr)
        {
            fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == nullptr)
            fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

        SDL_FreeSurface(surface);
        if (used_textures[i] != nullptr) {
            SDL_DestroyTexture(used_textures[i]);
        }
        used_textures[i] = texture;

        ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)32, (float)32));
        if ((i+1)%6!=0) ImGui::SameLine();
    }
    ImGui::End();
}

void Debugger::render_game() {
    ImGui::Begin("Game");
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)ppu.get_frame_buffer(), 160, 144, 8, 160, 0x0, 0x0, 0x0, 0x0);
    SDL_SetPaletteColors(surface->format->palette, colors, 0 ,4);
    if (surface == nullptr)
    {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

    SDL_FreeSurface(surface);
    if (old_game_texture != nullptr) {
        SDL_DestroyTexture(old_game_texture);
    }
    old_game_texture = texture;

    ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)160*3, (float)144*3));
    ImGui::End();
}

void Debugger::capture_keyboard() {
    memset(Joypad::key_state, 0, 8);
    const Uint8* key = SDL_GetKeyboardState(NULL);
    if(key[SDL_SCANCODE_Y]) Joypad::key_state[0]=1;
    if(key[SDL_SCANCODE_T]) Joypad::key_state[1]=1;
    if(key[SDL_SCANCODE_M]) Joypad::key_state[2]=1;
    if(key[SDL_SCANCODE_N]) Joypad::key_state[3]=1;
    if(key[SDL_SCANCODE_S]) Joypad::key_state[4]=1;
    if(key[SDL_SCANCODE_W]) Joypad::key_state[5]=1;
    if(key[SDL_SCANCODE_A]) Joypad::key_state[6]=1;
    if(key[SDL_SCANCODE_D]) Joypad::key_state[7]=1;
}

void Debugger::log(std::string s) {
   if (debug_buffer.size() >= 1000) {
       debug_buffer.pop_front();
   }
    debug_buffer.push_back(s);
}


void Debugger::end() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


