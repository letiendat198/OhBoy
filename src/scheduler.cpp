#include <debugger.h>
#include <dma.h>
#include <interrupts.h>
#include <joypad.h>
#include <scheduler.h>

Scheduler::Scheduler() {
    schedule(SchedulerEvent::OAM_SCAN, 0);
    schedule(SchedulerEvent::NEW_LINE, 114);
    schedule(SchedulerEvent::DIV_OVERFLOW, DIV_OVERFLOW_CYCLE);

    Timer::current_tac = Timer::read_tac(Memory::read(0xFF07));
}

// Schedule an event a number of cycle from this point on
void Scheduler::schedule(SchedulerEvent event, uint32_t cycle_to_go) {
    // if (static_cast<int>(event)>=5 && CPU::double_spd_mode) cycle_to_go /= 4;
    // else cycle_to_go /= 2;
    SchedulerEventInfo event_info{event, cycle_to_go + current_cycle};
    event_queue.insert(event_info);
    // logger.get_logger()->debug("Schedule event: {:d} for cycle: {:d}", static_cast<int>(event), cycle_to_go + current_cycle);
}

// Schedule an event at this exact cycle
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

SchedulerEventInfo Scheduler::get_schedule(SchedulerEvent event) {
    for(auto iter = event_queue.begin(); iter != event_queue.end(); ++iter) {
        if (iter->event == event) return *iter;
    }
    return SchedulerEventInfo{};
}

// Delay a schedule a number of cycle
void Scheduler::delay_schedule(SchedulerEvent event, uint32_t cycle_to_delay) {
    SchedulerEventInfo event_info = get_schedule(event);
    if (event_info.event == ILLEGAL) return;
    remove_schedule(event);
    schedule_absolute(event, event_info.cycle + cycle_to_delay);
}

bool interrupt_already_fired = false;

SchedulerEventInfo Scheduler::progress() {
    if (event_queue.begin() == event_queue.end()) {
        logger.get_logger()->debug("Event queue empty!");
    }
    // logger.get_logger()->debug("Next event: {:d} at cycle: {:d}", static_cast<int>(event_queue.begin()->event), event_queue.begin()->cycle);
    while (true) {
        Joypad::tick();
        if (current_cycle + cpu.fetch_next_length() > event_queue.begin()->cycle) {
            cpu.handle_interrupts();
            interrupt_already_fired = true;
            break;
        }
        if (!interrupt_already_fired) cpu.handle_interrupts();
        current_cycle += cpu.tick();
        // In double speed mode, CPU and DIV/TIMA should be twice as fast
        // But since the latter is inaccurate (for now) and most games don't relly on them
        // So I can get away with just ticking the CPU once more (Not cycle accurate though).
        if (CPU::double_spd_mode) cpu.tick();
        interrupt_already_fired = false;
    }
    logger.get_logger()->debug("Current DIV: {:d}. Current cycle: {:d}. Overflow cycle: {:d}", Timer::calc_current_div(), current_cycle, Timer::div_overflow_cycle);
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
                ppu.draw_scanline();
                ppu.update_stat(0);
                ppu.schedule_next_mode(0);
                break;
            case VBLANK:
                ppu.window_ly = 0;
                Interrupts::set_interrupt_flag(0);
                ppu.update_stat(1);
                ppu.schedule_next_mode(1);
                if (debugger != nullptr) debugger->render();
                break;
            case NEW_LINE:
                ppu.update_ly();
                // ppu.update_stat(0);
                schedule(NEW_LINE, 114);
                break;
            case DMA_TRANSFER:
                DMA::transfer_dma();
                break;
            case DIV_OVERFLOW:
                schedule(DIV_OVERFLOW, DIV_OVERFLOW_CYCLE);
                Timer::div_overflow_cycle = current_cycle;
                break;
            case TIMA_TICK:
                Interrupts::set_interrupt_flag(2);
                Timer::schedule_next_tima_overflow(Memory::read(0xFF06));
                break;
            case GDMA_TRANSFER:
                HDMA::transfer_gdma();
                break;
            case HDMA_TRANSFER:
                HDMA::transfer_hdma();
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

        if (event_info.event == TIMA_OVERFLOW && Timer::next_tima_overflow_cycle > CYCLE_PER_FRAME) Timer::next_tima_overflow_cycle -= CYCLE_PER_FRAME;
        temp.insert(event_info);
    }
    event_queue.swap(temp);
}

void Scheduler::set_render_callback(Debugger *debugger) {
    this->debugger = debugger;
}



