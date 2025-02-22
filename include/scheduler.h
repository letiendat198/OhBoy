#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cpu.h>
#include <ppu.h>
#include <apu.h>

#define NO_EVENT_SCHEDULED UINT32_MAX

class Debugger;

enum EVENT_ID: uint8_t {
    SAMPLE_APU,

    // PPU
    HBLANK,
    VBLANK,
    OAM_SCAN,
    DRAW,
    // Anything UP is not affected by double speed mode

    // TIMER
    DIV_OVERFLOW,
    TIMA_OVERFLOW,

    // APU
    DIV_APU_TICK,
    SQUARE1_PERIOD_OVERFLOW,
    SQUARE2_PERIOD_OVERFLOW,
    WAVE_PERIOD_OVERFLOW,
    NOISE_PERIOD_OVERFLOW,

    ILLEGAL
};

struct SchedulerEvent {
    EVENT_ID event_id = ILLEGAL;
    uint32_t cycle = NO_EVENT_SCHEDULED;
    uint32_t relative_cycle = NO_EVENT_SCHEDULED;
    bool operator < (SchedulerEvent a) const {
        if (cycle == a.cycle) return event_id < a.event_id; // If 2 event occur on same cycle, sort by priority
        return cycle < a.cycle;
    }
};

class Scheduler {
private:
    inline static const uint8_t MAX_EVENT = ILLEGAL + 1;
    inline static SchedulerEvent *event_queue = new SchedulerEvent[MAX_EVENT+1];
    inline static SchedulerEvent *next_event = nullptr;

    Debugger *debugger = nullptr;
public:
    inline static Logger logger = Logger("Scheduler");
    CPU cpu;
    PPU ppu;
    APU apu;

    inline static uint32_t current_cycle = 0;
    bool pause = false;

    Scheduler();
    static void schedule(EVENT_ID event_id, uint32_t cycle_to_go);
    static void schedule_absolute(EVENT_ID event_id, uint32_t cycle);
    static void remove_schedule(EVENT_ID event_id);
    static void remove_ppu_schedules();
    static void reschedule(EVENT_ID event, uint32_t cycle);
    static void find_next_event();
    static void switch_speed(bool is_double_spd);
    SchedulerEvent progress();
    void tick_frame();

    void set_debugger(Debugger *debugger);
};

#endif //SCHEDULER_H
