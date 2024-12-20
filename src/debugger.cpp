#include "../include/debugger.h"

#include <config.h>
#include <cstdio>
#include <format>
#include <imgui_internal.h>

#include "dma.h"
#include "joypad.h"

void Debugger::init(bool debug) {
    is_debug = debug;
    is_cpu_paused = debug;
    cpu.init(Cartridge::cgb_mode);
    ppu.init(Cartridge::cgb_mode);
    apu.init();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    int width = 160*3 + 18;
    int height = 144*3 + 18;
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

    SDL_AudioSpec spec;
    spec.freq = OUTPUT_FREQUENCY;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = SAMPLE_COUNT;
    spec.callback = &audio_callback;

    SDL_AudioSpec obtained;

    audioDeviceID = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);
    SDL_PauseAudioDevice(audioDeviceID, 0);

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

void Debugger::tick_cpu() {
    if (!is_cpu_paused) {
        for(int i=0;i<4;i++) {
            ppu.tick();
            apu.tick();
            if (i==1 && cpu.double_spd_mode) {
                DMA::tick();
                timer.tick();
                cpu.tick();
            }
        }

        Joypad::tick();

        cpu.tick();
        DMA::tick();
        if (!cpu.halt || (Memory::check_hdma() && Memory::get_hdma_type()==0)) HDMA::tick();
        timer.tick();

        uint8_t serial_data = Memory::read(0xFF01);
        if (Memory::read(0xFF02) == 0x81 && serial_data >= 32 && serial_data <= 127) {
            serial_output += serial_data;
            Memory::write(0xFF02, 0x01);
        }
    }
    if ((breakpoint != 0 && cpu.pc == breakpoint)) {
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

    ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode, 0);

    render_game();

    if (is_debug) {
        render_console(io);
        render_registers();
        memory_editor.ReadOnly = true;
        memory_editor.DrawWindow("Memory Bus", Memory::get_raw(), 0x4000);
        memory_editor.DrawWindow("External RAM", Cartridge::external_ram, Cartridge::external_ram_size);
        memory_editor.DrawWindow("Video RAM", Memory::get_raw_vram(), 0x2000*2);
        memory_editor.DrawWindow("Work RAM 1-7", Memory::get_raw_wram(), 0x1000*7);
        memory_editor.DrawWindow("Frame Buffer", PPU::get_frame_buffer(), 160*144*3);
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
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)ppu.get_frame_buffer(), 160, 144, 24, 160*3, 0x0, 0x0, 0x0, 0x0);
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

    ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)160*3, (float)144*3));
    ImGui::End();
}

void Debugger::render_console(const ImGuiIO& io) {
    ImGui::Begin("Debug Info");
    ImGui::Text("Frame rate:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(io.Framerate).c_str());
    // if(ImGui::Button("Flush log")) {
    //     cpu.logger.flush();
    // }
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
    ImGui::InputInt("Breakpoint", &breakpoint);
    if (cpu.pc == breakpoint) {
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

void audio_callback(void *userdata, Uint8 *stream, int len) {
    for(int i=0;i<len/sizeof(short);i++) {
        stream[i*2] = APU::sample_buffer[i] & 0xFF;
        stream[i*2 + 1] = (APU::sample_buffer[i] & 0xFF00) >> 8;
    }
    APU::clear_sample_queue();
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


