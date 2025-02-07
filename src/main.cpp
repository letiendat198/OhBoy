#define SDL_MAIN_HANDLED
#include <iostream>
#include <chrono>
#include <popl.hpp>
#include "debugger.h"
#include "config.h"
#include "scheduler.h"
#include "spdlog/sinks/basic_file_sink.h"

using namespace std;

void measure_frame_without_render(Scheduler *scheduler) {
    long long frames = 0;
    auto t1 = std::chrono::steady_clock::now();
    while (true){
        scheduler->tick_frame();
        frames++;
        auto t2 = std::chrono::steady_clock::now(); // Capture render + cycle time
        double elapse = chrono::duration<double, std::milli>(t2-t1).count();
        if (elapse/1000 > 5) {
            std::cout<<"Frames count: "<<frames<<" Seconds: "<<elapse / 1000<<" Average FPS: "<<frames / (elapse/1000)<<"\n";
            break;
        }
    }
}

Scheduler scheduler;
int main(int argc , char **argv){
    popl::OptionParser op("Allowed Options");
    auto help_options = op.add<popl::Switch>("h", "help", "Get help");
    auto rom_path_option = op.add<popl::Value<std::string>>("r", "rom", "path to rom file");
    auto debug_option = op.add<popl::Switch>("d", "debug", "Enable debugging");
    op.parse(argc, argv);

    bool debug_mode = false;
    if (debug_option->is_set()) {
        debug_mode = true;
    }
    if (!rom_path_option->is_set()) {
        cerr<<"No rom specified\n";
        return -1;
    }

    cout<<rom_path_option->value().c_str()<<endl;
    bool cart_init = Memory::cartridge.init(rom_path_option->value().c_str());
    if (!cart_init) return -1;

    Memory::scheduler = &scheduler;
    scheduler.ppu.set_cgb_mode(Memory::cartridge.is_cgb);
    // measure_frame_without_render(&scheduler);
    Debugger debugger(&scheduler, debug_mode);
    scheduler.set_render_callback(&debugger);

    auto t1 = std::chrono::steady_clock::now();
    while (!debugger.done){
        scheduler.tick_frame();
        auto t2 = std::chrono::steady_clock::now(); // Capture render + cycle time
        double elapse = chrono::duration<double, std::milli>(t2-t1).count();
        debugger.last_frame_duration = elapse;

        // if (elapse < MS_PER_FRAME) { // If still have some time left in this frame -> Sleep
        //     std::this_thread::sleep_for(chrono::duration<double, milli>(MS_PER_FRAME - elapse));
        // }
        // t1 = std::chrono::steady_clock::now(); // Capture time at the start of new frame

        // debugger.render();
        if (scheduler.apu.sample_count == SAMPLE_COUNT) {
            debugger.queue_audio();
            scheduler.apu.sample_count = 0;
        }
        debugger.capture_keyboard();
    }
    debugger.end();
    Memory::cartridge.save_sram();
    return 0;
}