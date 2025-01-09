#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <apu.h>
#include <cpu.h>
#include <ppu.h>
#include <set>
#include <timer.h>
#include <config.h>
#include <queue>

class Debugger;

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

    // TIMER
    DIV_TICK,
    TIMA_TICK
};

struct SchedulerEventInfo {
    SchedulerEvent event;
    uint32_t cycle;
    bool double_spd_affected;
    bool operator < (SchedulerEventInfo a) const {
        if (cycle == a.cycle) return event < a.event; // If 2 event occur on same cycle, sort by priority
        return cycle < a.cycle;
    }
};

class Scheduler {
private:
    inline static std::set<SchedulerEventInfo> event_queue;
    Timer timer;

    Debugger *debugger = nullptr;
public:
    inline static Logger logger = Logger("Scheduler");
    CPU cpu;
    PPU ppu;

    inline static uint32_t current_cycle = 0;

    bool pause = false;

    Scheduler();
    static void schedule(SchedulerEvent event, uint32_t cycle);
    static void remove_schedule(SchedulerEvent event);
    static void reschedule(SchedulerEvent event, uint32_t cycle);
    SchedulerEventInfo progress();
    void tick_frame();

    void set_render_callback(Debugger *debugger);
};

#endif //SCHEDULER_H
