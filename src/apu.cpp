#include "apu.h"
#include <cpu.h>
#include "scheduler.h"

void APU::sample() {
    short sample1 = channel1.is_enabled?channel1.sample:0;
    short sample2 = channel2.is_enabled?channel2.sample:0;
    short sample3 = channel3.is_enabled?channel3.sample:0;
    // uint8_t sample4 = channel4.enabled?channel4.get_current_sample():0;

    short sample = (sample1 + sample2 +sample3) * 100; // If this overflow, will cause crackling noise

    sample_output[sample_count++] = sample;
}

void APU::schedule_div_apu() {
    uint16_t freq = CPU::double_spd_mode ? 4096 : 2048;
    uint16_t offset = Timer::calc_current_div() % freq;

    Scheduler::reschedule(DIV_APU_TICK, freq - offset);
}

void APU::on_div_apu_tick() {
    div_apu_cycle = (div_apu_cycle + 1) % 8;
    if (div_apu_cycle % 2 == 0) {
        channel1.tick_length_timer();
        channel2.tick_length_timer();
        channel3.tick_length_timer();
        // channel4.tick_length_timer();
    }
    // if (div_apu_cycle == 2 || div_apu_cycle == 6) {
    //     channel1.tick_period_sweep();
    // }
    if (div_apu_cycle == 7) {
        channel1.tick_volume_envelope();
        channel2.tick_volume_envelope();
        // channel4.tick_volume_env();
    }
}

SquareWaveChannel::SquareWaveChannel(uint8_t channel_num): logger(Logger("SQUARE_CHANNEL_"+std::to_string(channel_num))) {
    PERIOD_OVERFLOW_EVENT = channel_num == 1 ? SQUARE1_PERIOD_OVERFLOW : SQUARE2_PERIOD_OVERFLOW;
}

void SquareWaveChannel::trigger() {
    is_enabled = is_dac_enabled;
    volume = (NRx2 >> 4) & 0xF;
    if (length_counter > LENGTH_OVERFLOW) length_counter = NRx1 & 0x3F;
    volume_envelope_counter = 0;

    if (!is_enabled) return;
    uint16_t freq = 0x800 - (((NRx4 & 0x7) << 8) | NRx3);
    Scheduler::reschedule(PERIOD_OVERFLOW_EVENT, freq); // VERY IMPORTANT TO RE-SCHEDULE. OTHERWISE, MULTIPLE OVERFLOW MAKE PITCH THROUGH THE ROOF
}

void SquareWaveChannel::disable() {
    is_enabled = false;
    Scheduler::remove_schedule(PERIOD_OVERFLOW_EVENT);
}

void SquareWaveChannel::on_period_overflow() {
    uint8_t duty = (NRx1 >> 6) & 0x3;
    sample = ((square_wave[duty] >> (7-duty_step)) & 0x1) * volume;
    duty_step = (duty_step + 1) & 0x7;

    uint16_t freq = 0x800 - (((NRx4 & 0x7) << 8) | NRx3);
    Scheduler::schedule(PERIOD_OVERFLOW_EVENT, freq);
}

void SquareWaveChannel::tick_length_timer() {
    if (NRx4 >> 6 & 0x1) length_counter++;
    if (length_counter > LENGTH_OVERFLOW) disable();
}

void SquareWaveChannel::tick_volume_envelope() {
    uint8_t sweep_pace = NRx2 & 0x7;
    uint8_t env_direction = NRx2 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_envelope_counter = (volume_envelope_counter+1) % 8;
        if (volume_envelope_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}


void WaveChannel::trigger() {
    is_enabled = is_dac_enabled;
    if (length_counter > LENGTH_OVERFLOW) length_counter = NRx1;
    volume = (NRx2 >> 5) & 0x3;
    wave_ram_step = 1;

    if (!is_enabled) return;
    uint16_t freq = 0x400 - (((NRx4 & 0x7) << 8) | NRx3) / 2;
    Scheduler::reschedule(WAVE_PERIOD_OVERFLOW, freq);
}

void WaveChannel::disable() {
    is_enabled = false;
    Scheduler::remove_schedule(WAVE_PERIOD_OVERFLOW);
}

void WaveChannel::on_period_overflow() {
    uint16_t wram_addr = 0xFF30;
    uint8_t current_sample_byte = Memory::unsafe_read(wram_addr + (wave_ram_step >> 1));

    uint8_t current_nibble = (current_sample_byte >> 4) & 0xF;
    if (wave_ram_step % 2 == 1) current_nibble = current_sample_byte & 0xF;

    if (volume != 0) sample = current_nibble >> (volume - 1);
    else sample = 0;

    // logger.get_logger()->debug("Current sample: {:X}", sample);

    wave_ram_step = (wave_ram_step + 1) % 32;

    uint16_t freq = 0x400 - ((NRx4 & 0x7) << 8 | NRx3) / 2;
    Scheduler::schedule(WAVE_PERIOD_OVERFLOW, freq);
}

void WaveChannel::tick_length_timer() {
    if (NRx4 >> 6 & 0x1) length_counter++;
    if (length_counter > LENGTH_OVERFLOW) disable();
}
