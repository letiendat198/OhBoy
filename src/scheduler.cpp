#include <dma.h>
#include <interrupts.h>
#include <joypad.h>
#include <scheduler.h>

Scheduler::Scheduler() {
    schedule(SchedulerEvent::OAM_SCAN, 0);
    schedule(SchedulerEvent::NEW_LINE, 114);
    schedule(SchedulerEvent::DIV_TICK, 64);
    schedule(SchedulerEvent::TIMA_TICK, 0); // Probably wrong
}

void Scheduler::schedule(SchedulerEvent event, uint32_t cycle_to_go) {
    SchedulerEventInfo event_info{event, cycle_to_go + current_cycle};
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

void Scheduler::reschedule(SchedulerEvent event, uint32_t cycle) {
    remove_schedule(event);
    schedule(event, cycle);
}

SchedulerEventInfo Scheduler::progress() {
    if (event_queue.begin() == event_queue.end()) {
        logger.get_logger()->debug("Event queue empty!");
    }
    logger.get_logger()->debug("Next event: {:d} at cycle: {:d}", static_cast<int>(event_queue.begin()->event), event_queue.begin()->cycle);
    while(current_cycle < event_queue.begin()->cycle) {
        if (!pause) current_cycle += cpu.tick();
        else {
            current_cycle += 100;
        }
        // Joypad need to tick every cycle because game will read and reset register value
        // Actual keyboard update can be done once per frame. Can probably optimize by tick per joypad register read
        Joypad::tick();
    }
    return event_queue.extract(event_queue.begin()).value();
}

void Scheduler::tick_frame() {
    // WARN: Scheduler may not clear event with cycle before limit if current cycle hit turn around limit
    while (current_cycle < CYCLE_PER_FRAME || event_queue.begin()->cycle <= CYCLE_PER_FRAME) {
        SchedulerEventInfo event_info = this->progress();
        uint32_t cycle_backup = current_cycle;
        current_cycle = event_info.cycle; // Set cycle context to the cycle event supposed to happen
        switch (event_info.event) {
            case OAM_SCAN:
                ppu.mode = 2;
                ppu.oam_scan();
                ppu.update_stat();
                ppu.schedule_next_mode(2);
                break;
            case DRAW:
                ppu.mode = 3;
                ppu.update_stat();
                ppu.schedule_next_mode(3);
                break;
            case HBLANK:
                ppu.mode = 0;
                ppu.draw_scanline();
                ppu.update_stat();
                ppu.schedule_next_mode(0);
                break;
            case VBLANK:
                ppu.mode = 1;
                ppu.window_ly = 0;
                Interrupts::set_interrupt_flag(0);
                ppu.update_stat();
                ppu.schedule_next_mode(1);
                break;
            case NEW_LINE:
                ppu.update_ly();
                ppu.update_stat();
                schedule(NEW_LINE, 114);
                break;
            case DMA_TRANSFER:
                DMA::transfer();
                break;
            case DIV_TICK:
                timer.tick_div();
                break;
            case TIMA_TICK:
                timer.tick_tima();
                break;
            default:
                logger.get_logger()->debug("Not yet implemented event");
        }
        current_cycle = cycle_backup; // Restore context
    }
    logger.get_logger()->debug("Turn around at cycle: {:d}", current_cycle);
    current_cycle -= CYCLE_PER_FRAME;
    std::set<SchedulerEventInfo> temp;
    for(auto event_info : event_queue) {
        if (event_info.cycle > CYCLE_PER_FRAME) event_info.cycle -= CYCLE_PER_FRAME;
        else logger.get_logger()->warn("Scheduler will miss event: {:d} at: {:d}", static_cast<int>(event_info.event), event_info.cycle);
        temp.insert(event_info);
    }
    event_queue.swap(temp);
}



