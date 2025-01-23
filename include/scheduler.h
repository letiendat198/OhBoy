#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cpu.h>
#include <ppu.h>
#include <set>
#include <timer.h>

typedef void(* hw_render_callback)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *buffer);

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
    hw_render_callback render_callback;
public:
    CPU cpu;
    PPU ppu;

    inline static uint32_t current_cycle = 0;
    inline static bool double_spd = false;
    bool pause = false;

    Scheduler();
    static void schedule(SchedulerEvent event, uint32_t cycle_to_go);
    static void schedule_absolute(SchedulerEvent event, uint32_t cycle);
    static void remove_schedule(SchedulerEvent event);
    static void reschedule(SchedulerEvent event, uint32_t cycle);
    static void switch_speed(bool is_double_spd);
    SchedulerEventInfo progress();
    void tick_frame();
    void set_render_callback(hw_render_callback render_callback);
};

#endif //SCHEDULER_H
