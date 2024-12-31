#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <apu.h>
#include <cpu.h>
#include <ppu.h>
#include <set>
#include <timer.h>
#include <config.h>
#include <queue>

enum SchedulerEvent {
    // PPU
    NEW_LINE,
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
    uint32_t cycle;
    bool operator < (SchedulerEventInfo a) const {
        if (cycle == a.cycle) return event < a.event; // If 2 event occur on same cycle, sort by priority
        return cycle < a.cycle;
    }
};

class Scheduler {
private:
    inline static std::set<SchedulerEventInfo> event_queue;

    Timer timer;
public:
    inline static Logger logger = Logger("Scheduler");
    CPU cpu;
    PPU ppu;
    APU apu;

    inline static uint32_t current_cycle = 0;

    Scheduler();
    static void schedule(SchedulerEvent event, uint32_t cycle);
    static void remove_schedule(SchedulerEvent event);
    static void reschedule(SchedulerEvent event, uint32_t cycle);
    SchedulerEventInfo progress();
    void tick_frame();
};

#endif //SCHEDULER_H
