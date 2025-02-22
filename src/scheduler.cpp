#include <debugger.h>
#include <dma.h>
#include <interrupt.h>
#include <timer.h>
#include <scheduler.h>

Scheduler::Scheduler() {
    // Pre-fill event slots with correct event id for easy comparison
    for(uint8_t i=0;i<MAX_EVENT;i++) event_queue[i].event_id = static_cast<EVENT_ID>(i);

    Timer::current_tac = Timer::read_tac(Memory::read(0xFF07));
    Timer::schedule_next_div_overflow();

    apu.schedule_div_apu();
    schedule(SAMPLE_APU, CYCLE_PER_SAMPLE);
}

// Schedule an event a number of cycle from this point on
void Scheduler::schedule(EVENT_ID event_id, uint32_t cycle_to_go) {
    // Only OAM SCAN should be schedule for 0 cycle (PPU enable)
    // Other events with self-scheduling property will hang emulator
    assert(event_id == OAM_SCAN || cycle_to_go != 0);
    assert(cycle_to_go != NO_EVENT_SCHEDULED);
    assert(("Only one event of a type should be scheduled!", event_queue[event_id].cycle == NO_EVENT_SCHEDULED));

    if (CPU::double_spd_mode && static_cast<int>(event_id)<=DRAW)
        event_queue[event_id].cycle = cycle_to_go * 2 + current_cycle;
    else event_queue[event_id].cycle = cycle_to_go + current_cycle;

    event_queue[event_id].relative_cycle = cycle_to_go;

    if (next_event == nullptr || event_queue[event_id] < *next_event) next_event = &event_queue[event_id];

    // logger.get_logger()->debug("Schedule event: {:d} for cycle: {:d}", static_cast<int>(event), cycle_to_go + current_cycle);
}

// Schedule an event at this exact cycle
// Must only be used to schedule TIMA
// Will have a problem when switching speed mode if scheduling different events cause no relative cycle info available
void Scheduler::schedule_absolute(EVENT_ID event_id, uint32_t cycle) {
    assert(cycle != NO_EVENT_SCHEDULED);
    assert(("Schedule absolute MUST NOT be used to schedule event OTHER THAN TIMA", event_id == TIMA_OVERFLOW));
    assert(("Only one event of a type should be scheduled!", event_queue[event_id].cycle == NO_EVENT_SCHEDULED));

    event_queue[event_id].cycle = cycle;
    if (next_event == nullptr ||  event_queue[event_id] < *next_event) next_event = &event_queue[event_id];
}


void Scheduler::remove_schedule(EVENT_ID event_id) {
    event_queue[event_id].cycle = NO_EVENT_SCHEDULED;
    if (next_event == &event_queue[event_id]) find_next_event();
}

void Scheduler::remove_ppu_schedules() {
    for (uint8_t i = HBLANK; i<=DRAW; i++) {
        event_queue[i].cycle = NO_EVENT_SCHEDULED;
    }
    find_next_event();
}

// Re-schedule an event (cycle) into the future from this point on
void Scheduler::reschedule(EVENT_ID event, uint32_t cycle) {
    remove_schedule(event);
    schedule(event, cycle);
}

void Scheduler::find_next_event() {
    for (uint8_t i = 0; i<MAX_EVENT;i++) {
        if (event_queue[i].cycle != NO_EVENT_SCHEDULED && (next_event == nullptr || event_queue[i] < *next_event)) next_event = &event_queue[i];
    }
}


void Scheduler::switch_speed(bool is_double_spd) {
    CYCLE_PER_FRAME = is_double_spd ? 17556*2 : 17556;
    logger.get_logger()->debug("Speed switch at cycle: {:d}", current_cycle);

    if (is_double_spd) {
        for(uint8_t i = 0; i <= DRAW; i++) {
            if (event_queue[i].cycle == NO_EVENT_SCHEDULED) continue;
            event_queue[i].cycle += event_queue[i].relative_cycle;
            logger.get_logger()->debug("Reschedule event: {:d} from cycle: {:d} to {:d}", i, event_queue[i].cycle - event_queue[i].relative_cycle, event_queue[i].cycle);
        }
    }
    else {
        for(uint8_t i = 0; i <= DRAW; i++) {
            if (event_queue[i].cycle == NO_EVENT_SCHEDULED) continue;
            event_queue[i].cycle -= event_queue[i].relative_cycle;
        }
    }

    find_next_event();
}


SchedulerEvent Scheduler::progress() {
    assert(next_event != nullptr);
    // logger.get_logger()->debug("Next event: {:d} at cycle: {:d}", static_cast<int>(event_queue.begin()->event), event_queue.begin()->cycle);
    while(current_cycle < next_event->cycle) {
        cpu.handle_interrupts();
        if (cpu.halt) { // Skip to next event if halt, cause CPU do nothing anyway
            current_cycle = next_event->cycle;
            break;
        }
        current_cycle += cpu.tick();
    }
    SchedulerEvent old_event = *next_event;
    next_event->cycle = NO_EVENT_SCHEDULED;
    find_next_event();
    return old_event;
}

void Scheduler::tick_frame() {
    // WARN: Scheduler may not clear event with cycle before limit if current cycle hit turn around limit
    while (current_cycle < CYCLE_PER_FRAME || next_event->cycle <= CYCLE_PER_FRAME) {
        SchedulerEvent event_info = this->progress();
        uint32_t cycle_backup = current_cycle;
        current_cycle = event_info.cycle; // Set cycle context to the cycle event supposed to happen
        switch (event_info.event_id) {
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
                ppu.draw_scanline();
                PPU::mode = 0;
                if (HDMA::is_hdma_running && HDMA::hdma_type == 1 && !cpu.halt) HDMA::transfer_hdma();
                PPU::check_stat_interrupt();
                ppu.schedule_next_mode(0);
                break;
            case VBLANK:
                PPU::ly = (PPU::ly + 1) % 154; // Update LY only update LY register
                if (PPU::ly == 144) {
                    PPU::mode = 1;
                    ppu.window_ly = 0;
                    Interrupt::set_flag(VBLANK_INTR);
                    if (debugger != nullptr) debugger->render(); // WILL HANG IF PPU IS OFF
                }
                PPU::check_stat_interrupt();
                ppu.schedule_next_mode(1);
                break;
            case DIV_OVERFLOW:
                Timer::schedule_next_div_overflow();
                break;
            case TIMA_OVERFLOW:
                Interrupt::set_flag(TIMER_INTR);
                Timer::schedule_tima_overflow(Timer::tma);
                break;
            case DIV_APU_TICK:
                apu.on_div_apu_tick();
                apu.schedule_div_apu();
                break;
            case SQUARE1_PERIOD_OVERFLOW:
                apu.channel1.on_period_overflow();
                break;
            case SQUARE2_PERIOD_OVERFLOW:
                apu.channel2.on_period_overflow();
                break;
            case WAVE_PERIOD_OVERFLOW:
                apu.channel3.on_period_overflow();
                break;
            case NOISE_PERIOD_OVERFLOW:
                apu.channel4.on_period_overflow();
                break;
            case SAMPLE_APU:
                schedule(SAMPLE_APU, CYCLE_PER_SAMPLE);
                apu.sample();
                if (apu.sample_count == SAMPLE_COUNT) {
                    if (debugger != nullptr) debugger->queue_audio();
                    apu.sample_count = 0;
                }
                break;
            default:
                logger.get_logger()->debug("Not yet implemented event");
        }
        current_cycle = cycle_backup; // Restore context
    }
    // logger.get_logger()->debug("Turn around at cycle: {:d}", current_cycle);
    current_cycle -= CYCLE_PER_FRAME;

    for(uint8_t i=0;i<MAX_EVENT;i++) {
        if (event_queue[i].cycle != NO_EVENT_SCHEDULED && event_queue[i].cycle > CYCLE_PER_FRAME) event_queue[i].cycle -= CYCLE_PER_FRAME;
        else logger.get_logger()->warn("Scheduler will miss event: {:d} at: {:d}", i, event_queue[i].cycle);
        if (i == TIMA_OVERFLOW && Timer::tima_overflow_cycle > CYCLE_PER_FRAME) Timer::tima_overflow_cycle -= CYCLE_PER_FRAME;
    }
}

void Scheduler::set_debugger(Debugger *debugger) {
    this->debugger = debugger;
}
