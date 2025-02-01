#include <debugger.h>
#include <dma.h>
#include <interrupts.h>
#include <joypad.h>
#include <scheduler.h>

Scheduler::Scheduler() {
    Timer::current_tac = Timer::read_tac(Memory::read(0xFF07));
    Timer::schedule_next_div_overflow();
}

// Schedule an event a number of cycle from this point on
void Scheduler::schedule(SchedulerEvent event, uint32_t cycle_to_go) {
    SchedulerEventInfo event_info;
    if (CPU::double_spd_mode && static_cast<int>(event)<=4) event_info = SchedulerEventInfo{event, cycle_to_go * 2 + current_cycle, cycle_to_go};
    else event_info = SchedulerEventInfo{event, cycle_to_go + current_cycle, cycle_to_go};
    event_queue.insert(event_info);
    // logger.get_logger()->debug("Schedule event: {:d} for cycle: {:d}", static_cast<int>(event), cycle_to_go + current_cycle);
}

// Schedule an event at this exact cycle
// Must only be used to schedule TIMA
// Will have a problem when switching speed mode if scheduling different events
void Scheduler::schedule_absolute(SchedulerEvent event, uint32_t cycle) {
    assert(("Schedule absolute MUST NOT be used to schedule event OTHER THAN TIMA", event == TIMA_OVERFLOW));
    SchedulerEventInfo event_info{event, cycle};
    event_queue.insert(event_info);
}


void Scheduler::remove_schedule(SchedulerEvent event) {
    for(auto iter = event_queue.begin(); iter != event_queue.end(); ++iter) {
        if (iter->event == event) {
            event_queue.erase(iter);
            return;
        }
    }
}

void Scheduler::remove_ppu_schedules() {
    std::set<SchedulerEventInfo> temp;
    for(auto event_info : event_queue) {
        if (event_info.event > DRAW) temp.insert(event_info);
    }
    event_queue.swap(temp);
}


// Re-schedule an event (cycle) into the future from this point on
void Scheduler::reschedule(SchedulerEvent event, uint32_t cycle) {
    remove_schedule(event);
    schedule(event, cycle);
}

void Scheduler::switch_speed(bool is_double_spd) {
    CYCLE_PER_FRAME = is_double_spd ? 17556*2 : 17556;
    std::set<SchedulerEventInfo> temp;
    logger.get_logger()->debug("Speed switch at cycle: {:d}", current_cycle);
    if (is_double_spd) {
        for(auto event_info : event_queue) {
            if (event_info.event <= DRAW) {
                event_info.cycle += event_info.relative_cycle;
                logger.get_logger()->debug("Reschedule event: {:d} from cycle: {:d} to {:d}", static_cast<int>(event_info.event), event_info.cycle-event_info.relative_cycle, event_info.cycle);
            }
            temp.insert(event_info);
        }
    }
    else {
     for(auto event_info : event_queue) {
         if (event_info.event <= DRAW) {
             event_info.cycle -= event_info.relative_cycle;
         }
         temp.insert(event_info);
     }
    }
    event_queue.swap(temp);
}


SchedulerEventInfo Scheduler::progress() {
    if (event_queue.begin() == event_queue.end()) {
        logger.get_logger()->debug("Event queue empty!");
    }
    // logger.get_logger()->debug("Next event: {:d} at cycle: {:d}", static_cast<int>(event_queue.begin()->event), event_queue.begin()->cycle);
    while(current_cycle < event_queue.begin()->cycle) {
        cpu.handle_interrupts();
        if (cpu.halt) { // Skip to next event if halt, cause CPU do nothing anyway
            current_cycle = event_queue.begin()->cycle;
            break;
        }
        current_cycle += cpu.tick();
    }
    // logger.get_logger()->debug("Current DIV: {:d}. Current cycle: {:d}. Overflow cycle: {:d}", Timer::calc_current_div(), current_cycle, Timer::div_overflow_cycle);
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
                if (!ppu.first_line) PPU::ly = (PPU::ly + 1) % 154;
                else ppu.first_line = false;
                PPU::mode = 2;
                PPU::check_stat_interrupt();
                ppu.oam_scan();
                ppu.schedule_next_mode(2);
                break;
            case DRAW:
                PPU::mode = 3;
                PPU::check_stat_interrupt();
                ppu.schedule_next_mode(3);
                break;
            case HBLANK:
                if (HDMA::is_hdma_running && HDMA::hdma_type == 1 && !cpu.halt) HDMA::transfer_hdma();
                ppu.draw_scanline();
                PPU::mode = 0;
                PPU::check_stat_interrupt();
                ppu.schedule_next_mode(0);
                break;
            case VBLANK:
                PPU::ly = (PPU::ly + 1) % 154; // Update LY only update LY register
                if (PPU::ly == 144) {
                    PPU::mode = 1;
                    ppu.window_ly = 0;
                    Interrupts::set_interrupt_flag(0);
                    if (debugger != nullptr) debugger->render(); // WILL HANG IF PPU IS OFF
                }
                PPU::check_stat_interrupt();
                ppu.schedule_next_mode(1);
                break;
            case DIV_OVERFLOW:
                Timer::schedule_next_div_overflow();
                break;
            case TIMA_OVERFLOW:
                Interrupts::set_interrupt_flag(2);
                Timer::schedule_tima_overflow(Timer::tma);
                break;
            default:
                logger.get_logger()->debug("Not yet implemented event");
        }
        current_cycle = cycle_backup; // Restore context
    }
    // logger.get_logger()->debug("Turn around at cycle: {:d}", current_cycle);
    current_cycle -= CYCLE_PER_FRAME;
    std::set<SchedulerEventInfo> temp;
    for(auto event_info : event_queue) {
        if (event_info.cycle > CYCLE_PER_FRAME) event_info.cycle -= CYCLE_PER_FRAME;
        else logger.get_logger()->warn("Scheduler will miss event: {:d} at: {:d}", static_cast<int>(event_info.event), event_info.cycle);
        if (event_info.event == TIMA_OVERFLOW && Timer::tima_overflow_cycle > CYCLE_PER_FRAME) Timer::tima_overflow_cycle -= CYCLE_PER_FRAME;
        temp.insert(event_info);
    }
    event_queue.swap(temp);
}

void Scheduler::set_render_callback(Debugger *debugger) {
    this->debugger = debugger;
}



