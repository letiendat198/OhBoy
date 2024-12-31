#include <scheduler.h>

Scheduler::Scheduler() {
    schedule(SchedulerEvent::OAM_SCAN, 0);
    schedule(SchedulerEvent::NEW_LINE, 114);
}

void Scheduler::schedule(SchedulerEvent event, uint16_t cycle_to_go) {
    SchedulerEventInfo event_info{event, static_cast<uint16_t>(cycle_to_go + current_cycle)};
    event_queue.insert(event_info);
    // logger.get_logger()->debug("Schedule event: {:d} for cycle: {:d}", static_cast<int>(event), cycle_to_go + current_cycle);
}

void Scheduler::remove_schedule(SchedulerEvent event) {
    for(auto iter = event_queue.begin(); iter != event_queue.end(); ++iter) {
        if (iter->event == event) {
            event_queue.erase(iter);
            return;
        }
    }
}

void Scheduler::reschedule(SchedulerEvent event, uint16_t cycle) {
    remove_schedule(event);
    schedule(event, cycle);
}

SchedulerEventInfo Scheduler::progress() {
    if (event_queue.begin() == event_queue.end()) {
        logger.get_logger()->debug("Event queue empty!");
    }
    logger.get_logger()->debug("Next event: {:d} at cycle: {:d}", static_cast<int>(event_queue.begin()->event), event_queue.begin()->cycle);
    while(current_cycle < event_queue.begin()->cycle) {
        current_cycle += cpu.tick();
    }
    return event_queue.extract(event_queue.begin()).value();
}

void Scheduler::tick_frame() {
    while (current_cycle < CYCLE_PER_FRAME && event_queue.begin()->cycle <= CYCLE_PER_FRAME) {
        SchedulerEventInfo event_info = this->progress();
        uint16_t cycle_backup = current_cycle;
        current_cycle = event_info.cycle; // Set cycle context to the cycle event supposed to happen
        switch (event_info.event) {
            case OAM_SCAN:
                ppu.oam_scan();
                ppu.schedule_next_mode(2);
                break;
            case DRAW:
                ppu.schedule_next_mode(3);
                break;
            case HBLANK:
                ppu.draw_scanline();
                ppu.schedule_next_mode(0);
                break;
            case VBLANK:
                ppu.window_ly = 0;
                ppu.schedule_next_mode(1);
                break;
            case NEW_LINE:
                ppu.update_ly();
                schedule(NEW_LINE, 114);
                break;
            default:
                logger.get_logger()->debug("Not yet implemented event");
        }
        current_cycle = cycle_backup; // Restore context
    }

    current_cycle = 0;
    for(auto iter = event_queue.begin(); iter!=event_queue.end(); ++iter) { // May be buggy
        SchedulerEventInfo event_info = event_queue.extract(iter).value();
        event_info.cycle -= CYCLE_PER_FRAME;
        event_queue.insert(event_info);
    }
}



