#include <scheduler.h>

void Scheduler::schedule(SchedulerEvent event, int cycle) {
    SchedulerEventInfo event_info{event, cycle};
    event_queue.insert(event_info);
    next_event = *event_queue.begin();
}

SchedulerEvent Scheduler::progress() {
    int next_event_cycle = next_event.cycle;
    while(current_cycle < next_event_cycle) {
        current_cycle += cpu.tick();
    }
    return next_event.event;
}

void Scheduler::tick_frame() {
    while (current_cycle < CYCLE_PER_FRAME && next_event.cycle <= CYCLE_PER_FRAME) {
        SchedulerEvent event = this->progress();
        switch (event) {
            default: ;
        }
    }
}



