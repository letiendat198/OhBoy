#define SDL_MAIN_HANDLED
#include <iostream>
#include <algorithm>
#include <chrono>

#include "debugger.h"
#include "config.h"

using namespace std;

int main(int, char **){
    cout<<"Starting GB emulator"<<endl;

    bool cart_init = Cartridge::init("../roms/07-jr,jp,call,ret,rst.gb");
    if (!cart_init) return -1;

    Cartridge::read_to_mem();

    Debugger debugger;
    debugger.init();
    int cycle = 0;
    auto t1 = std::chrono::steady_clock::now();
    while (!debugger.done){
        cycle++;
        debugger.tick_cpu();
        if (cycle==CYCLE_PRE_FRAME) {
            auto t2 = std::chrono::steady_clock::now(); // Capture render + cycle time
            double elapse = chrono::duration<double, std::milli>(t2-t1).count();
            if (elapse < MS_PER_FRAME) { // If still have some time left in this frame -> Sleep
                _sleep(MS_PER_FRAME - elapse);
            }
            t1 = std::chrono::steady_clock::now(); // Capture time at the start of new frame
            debugger.render();
            cycle = 0;
        }
    }
    return 0;
}