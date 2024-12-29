#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <apu.h>
#include <cpu.h>
#include <ppu.h>
#include <set>
#include <timer.h>
#include <config.h>

enum SchedulerEvent {
    // PPU
    HBLANK,
    VBLANK,
    OAM_SCAN,
    DRAW,

    // DMA
    DMA_TRANSFER,
    GDMA_TRANSFER,
    HDMA_TRANSFER,

    // INTERRUPT
    SERIAL_INTR,
    TIMER_INTR,
    STAT_INTR,
    VBLANK_INTR
};

struct SchedulerEventInfo {
    SchedulerEvent event;
    int cycle;
};

class Scheduler {
private:
    struct cmp {
        bool operator() (SchedulerEventInfo a, SchedulerEventInfo b) const {
            return a.cycle < b.cycle;
        }
    };

    inline static std::set<SchedulerEventInfo, cmp> event_queue;
    inline static SchedulerEventInfo next_event;

    Timer timer;
public:
    CPU cpu;
    PPU ppu;
    APU apu;

    int current_cycle = 0;

    static void schedule(SchedulerEvent, int cycle);
    SchedulerEvent progress();
    void tick_frame();
};

#endif //SCHEDULER_H
