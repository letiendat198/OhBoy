#include <scheduler.h>

int main() {
    Scheduler scheduler;
    Memory::cartridge.init("");
    Memory::scheduler = &scheduler;
    scheduler.ppu.set_cgb_mode(Memory::cartridge.is_cgb);

    while(true) {
        scheduler.tick_frame();
    }
}
