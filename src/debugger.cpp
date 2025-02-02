#include "../include/debugger.h"

#include <config.h>
#include <cstdio>
#include <format>
#include <imgui_internal.h>

#include "dma.h"
#include "joypad.h"

Debugger::Debugger(Scheduler *scheduler, bool debug) {
    is_debug = debug;
    this->scheduler = scheduler;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    int width = 160*scale + 18;
    int height = 144*scale + 18;
    if (is_debug) {
        width = 1280;
        height = 720;
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
    SDL_RenderSetVSync(renderer, 0);

    // SDL_AudioSpec spec;
    // spec.freq = OUTPUT_FREQUENCY;
    // spec.format = AUDIO_S16SYS;
    // spec.channels = 1;
    // spec.samples = SAMPLE_COUNT;
    // spec.callback = &audio_callback;
    // spec.userdata = &scheduler->apu;
    //
    // SDL_AudioSpec obtained;
    //
    // audioDeviceID = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);
    // SDL_PauseAudioDevice(audioDeviceID, 0);

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
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window)) {
        int x = event.window.data1;
        int y = event.window.data2;
        scale = std::min(x / 160, y / 144);
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode, 0);

    render_game();

    if (is_debug) {
        render_registers(io);
        memory_editor.ReadOnly = true;
        memory_editor.DrawWindow("Memory Bus", Memory::memory, 0x2000);
        memory_editor.DrawWindow("External RAM", Memory::cartridge.external_ram, Memory::cartridge.external_ram_size);
        memory_editor.DrawWindow("Video RAM", Memory::vram, 0x2000*2);
        memory_editor.DrawWindow("Work RAM 0-7", Memory::wram, 0x1000*8);
        memory_editor.DrawWindow("Frame Buffer", scheduler->ppu.frame_buffer, 160*144*3);
    }

    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, 115, 140, 153, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Debugger::render_game() {
    if (!is_debug) {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);
    }
    ImGui::Begin("Game");
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)scheduler->ppu.frame_buffer, 160, 144, 16, 160*2, 0x0, 0x0, 0x0, 0x0);
    if (surface == nullptr) {
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

    ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)160*scale, (float)144*scale));
    ImGui::End();
}

void Debugger::render_registers(const ImGuiIO& io) {
    ImGui::Begin("Registers");

    // if (ImGui::Button("Pause")) is_cpu_paused = true;
    // ImGui::SameLine();
    // if (ImGui::Button("Step")) cpu.tick();
    // ImGui::SameLine();
    // if (ImGui::Button("Continue")) {
    //     is_cpu_paused = false;
    // }

    // ImGui::InputInt("Breakpoint", &breakpoint);
    // if (cpu.pc == breakpoint) {
    //     ImGui::TextColored(ImVec4(255,255,0,255), "Breakpoint hit!");
    // }

    ImGui::Text("Screen FPS:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(io.Framerate).c_str());

    ImGui::Text("Frame FPS:");
    ImGui::SameLine();
    ImGui::Text(std::to_string((double)1000/last_frame_duration).c_str());

    CPU cpu = scheduler->cpu;

    ImGui::InputInt("Breakpoint", &breakpoint);
    if (cpu.pc == breakpoint) {
        ImGui::TextColored(ImVec4(255,255,0,255), "Breakpoint hit!");
        scheduler->pause = true;
    }
    if (ImGui::Button("Pause")) scheduler->pause = true;
    ImGui::SameLine();
    if (ImGui::Button("Step")) scheduler->cpu.tick();
    ImGui::SameLine();
    if (ImGui::Button("Continue")) scheduler->pause = false;

    if (cpu.halt) ImGui::TextColored(ImVec4(255,255,0,255), "CPU Halted!");
    else ImGui::Text("");

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

    ImGui::End();
}

void audio_callback(void *userdata, Uint8 *stream, int len) {
    // blip_read_samples(static_cast<APU*>(userdata)->blip, reinterpret_cast<short *>(stream), len / sizeof(short), 0);
    memcpy(stream, static_cast<APU*>(userdata)->blip_out, len);
}


void Debugger::capture_keyboard() {
    memset(Joypad::key_state, 0, 8);
    const Uint8* key = SDL_GetKeyboardState(NULL);
    if(key[SDL_SCANCODE_Y]) Joypad::key_state[0]=1;
    if(key[SDL_SCANCODE_T]) Joypad::key_state[1]=1;
    if(key[SDL_SCANCODE_N]) Joypad::key_state[2]=1;
    if(key[SDL_SCANCODE_M]) Joypad::key_state[3]=1;
    if(key[SDL_SCANCODE_S]) Joypad::key_state[4]=1;
    if(key[SDL_SCANCODE_W]) Joypad::key_state[5]=1;
    if(key[SDL_SCANCODE_A]) Joypad::key_state[6]=1;
    if(key[SDL_SCANCODE_D]) Joypad::key_state[7]=1;
}

void Debugger::end() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


