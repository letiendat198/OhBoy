#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cpu.h>
#include <ppu.h>
#include <set>
#include <timer.h>

class Debugger;

enum SchedulerEvent {
    // PPU
    HBLANK,
    VBLANK,
    OAM_SCAN,
    DRAW,

    // TIMER
    DIV_OVERFLOW,
    TIMA_OVERFLOW,

    ILLEGAL
};

struct SchedulerEventInfo {
    SchedulerEvent event = ILLEGAL;
    uint32_t cycle = 0;
    uint32_t relative_cycle = 0;
    bool operator < (SchedulerEventInfo a) const {
        if (cycle == a.cycle) return event < a.event; // If 2 event occur on same cycle, sort by priority
        return cycle < a.cycle;
    }
};

class Scheduler {
private:
    inline static std::set<SchedulerEventInfo> event_queue;

    Debugger *debugger = nullptr;
public:
    inline static Logger logger = Logger("Scheduler");
    CPU cpu;
    PPU ppu;

    inline static uint32_t current_cycle = 0;
    inline static bool double_spd = false;
    inline static bool first_line = true;
    bool pause = false;

    Scheduler();
    static void schedule(SchedulerEvent event, uint32_t cycle_to_go);
    static void schedule_absolute(SchedulerEvent event, uint32_t cycle);
    static void remove_schedule(SchedulerEvent event);
    static void remove_ppu_schedules();
    static void reschedule(SchedulerEvent event, uint32_t cycle);
    static void switch_speed(bool is_double_spd);
    SchedulerEventInfo progress();
    void tick_frame();

    void set_render_callback(Debugger *debugger);
};

#endif //SCHEDULER_H
