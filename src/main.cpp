#define SDL_MAIN_HANDLED
#include <iostream>
#include <algorithm>
#include <chrono>
#include <popl.hpp>

#include "debugger.h"
#include "config.h"
#include "spdlog/sinks/basic_file_sink.h"

using namespace std;

Debugger debugger;
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
    bool cart_init = Cartridge::init(rom_path_option->value().c_str());
    if (!cart_init) return -1;

    debugger.init(debug_mode);
    Debugger::log("Starting GB emulator");

    int cycle = 0;
    auto t1 = std::chrono::steady_clock::now();
    while (!debugger.done){
        cycle++;
        debugger.tick_cpu();
        if (cycle==CYCLE_PER_FRAME) {
            auto t2 = std::chrono::steady_clock::now(); // Capture render + cycle time
            double elapse = chrono::duration<double, std::milli>(t2-t1).count();
            if (elapse < MS_PER_FRAME) { // If still have some time left in this frame -> Sleep
                _sleep(MS_PER_FRAME - elapse);
            }
            t1 = std::chrono::steady_clock::now(); // Capture time at the start of new frame
            debugger.queue_audio();
            debugger.render();
            cycle = 0;
        }
    }
    debugger.end();
    Cartridge::close();
    return 0;
}