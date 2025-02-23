#include <scheduler.h>

int main() {
    Scheduler scheduler;
    scheduler.cpu.bus.cartridge.init("");
    scheduler.cpu.bus.ppu.set_cgb_mode(scheduler.cpu.bus.cartridge.is_cgb);

    while(true) {
        scheduler.tick_frame();
    }
}
