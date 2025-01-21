#include <dma.h>
#include <interrupts.h>
#include <joypad.h>
#include <scheduler.h>
#include <config.h>

Scheduler::Scheduler() {
    schedule(SchedulerEvent::OAM_SCAN, 0);
    schedule(SchedulerEvent::NEW_LINE, 114);

    Timer::current_tac = Timer::read_tac(Memory::read(0xFF07));
    Timer::schedule_next_div_overflow();
}

// Schedule an event a number of cycle from this point on
void Scheduler::schedule(SchedulerEvent event, uint32_t cycle_to_go) {
    SchedulerEventInfo event_info;
    if (double_spd && static_cast<int>(event)<=4) event_info = SchedulerEventInfo{event, cycle_to_go * 2 + current_cycle, cycle_to_go};
    else event_info = SchedulerEventInfo{event, cycle_to_go + current_cycle, cycle_to_go};
    event_queue.insert(event_info);
    // logger.get_logger()->debug("Schedule event: {:d} for cycle: {:d}", static_cast<int>(event), cycle_to_go + current_cycle);
}

// Schedule an event at this exact cycle
// Must only be used to schedule TIMA
// Will have a problem when switching speed mode if scheduling different events
void Scheduler::schedule_absolute(SchedulerEvent event, uint32_t cycle) {
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

// Re-schedule an event (cycle) into the future from this point on
void Scheduler::reschedule(SchedulerEvent event, uint32_t cycle) {
    remove_schedule(event);
    schedule(event, cycle);
}

void Scheduler::switch_speed(bool is_double_spd) {
    double_spd = is_double_spd;
    CYCLE_PER_FRAME = double_spd ? 17556*2 : 17556;
    std::set<SchedulerEventInfo> temp;
    // logger.get_logger()->debug("Speed switch at cycle: {:d}", current_cycle);
    if (double_spd) {
        for(auto event_info : event_queue) {
            if (event_info.event > 4) continue;
            event_info.cycle += event_info.relative_cycle;
            // logger.get_logger()->debug("Reschedule event: {:d} from cycle: {:d} to {:d}", static_cast<int>(event_info.event), event_info.cycle-event_info.relative_cycle, event_info.cycle);
            temp.insert(event_info);
        }
    }
    else {
        for(auto event_info : event_queue) {
            if (event_info.event > 4) continue;
            event_info.cycle -= event_info.relative_cycle;
            temp.insert(event_info);
        }
    }
    event_queue.swap(temp);
}

uint8_t frame_count = 0;

SchedulerEventInfo Scheduler::progress() {
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
                ppu.oam_scan();
                ppu.update_stat(2);
                ppu.schedule_next_mode(2);
                break;
            case DRAW:
                ppu.update_stat(3);
                ppu.schedule_next_mode(3);
                break;
            case HBLANK:
                if (Memory::check_hdma() && Memory::get_hdma_type() == 1 && !cpu.halt) schedule(HDMA_TRANSFER, 0);
                if (frame_count % 2 == 0) ppu.draw_scanline();
                ppu.update_stat(0);
                ppu.schedule_next_mode(0);
                break;
            case VBLANK:
                ppu.window_ly = 0;
                Interrupts::set_interrupt_flag(0);
                ppu.update_stat(1);
                ppu.schedule_next_mode(1);
                break;
            case NEW_LINE:
                ppu.update_ly();
                // ppu.update_stat(0);
                schedule(NEW_LINE, 114);
                break;
            case DIV_OVERFLOW:
                Timer::schedule_next_div_overflow();
                break;
            case TIMA_OVERFLOW:
                Interrupts::set_interrupt_flag(2);
                Timer::schedule_tima_overflow(Timer::tma);
                break;
            case DMA_TRANSFER:
                DMA::transfer_dma();
                break;
            case GDMA_TRANSFER:
                HDMA::transfer_gdma();
                break;
            case HDMA_TRANSFER:
                HDMA::transfer_hdma();
                break;
            default:
                printf("Not yet implemented event\n");
        }
        current_cycle = cycle_backup; // Restore context
    }
    // logger.get_logger()->debug("Turn around at cycle: {:d}", current_cycle);
    current_cycle -= CYCLE_PER_FRAME;
    std::set<SchedulerEventInfo> temp;
    for(auto event_info : event_queue) {
        if (event_info.cycle > CYCLE_PER_FRAME) event_info.cycle -= CYCLE_PER_FRAME;
        // else logger.get_logger()->warn("Scheduler will miss event: {:d} at: {:d}", static_cast<int>(event_info.event), event_info.cycle);
        if (event_info.event == TIMA_OVERFLOW && Timer::tima_overflow_cycle > CYCLE_PER_FRAME) Timer::tima_overflow_cycle -= CYCLE_PER_FRAME;
        temp.insert(event_info);
    }
    event_queue.swap(temp);

    frame_count++;
    if (frame_count==10) frame_count = 0;
}


