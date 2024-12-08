#include "apu.h"

#include <cpu.h>
#include <memory.h>

void APU::init(){

}

void APU::tick() {
    tick_div_apu();
    cycle++;
    if (cycle % 4 == 0) {
        channel1.tick(div_apu_cycle);
        channel2.tick(div_apu_cycle);
    }

    if (cycle == 87) {
        float sample_channel1 = channel1.dac_enabled?dac(channel1.current_sample):0;
        float sample_channel2 = channel2.dac_enabled?dac(channel2.current_sample):0;
        // logger.get_logger()->debug("Sample: {:.3f}", sample);
        float sample = sample_channel1 + sample_channel2;
        queue_sample(sample/100);
        cycle = 0;
    }
}

void APU::tick_div_apu() {
    uint8_t bit_shift = CPU::double_spd_mode?5:4;
    uint8_t div = Memory::unsafe_read(0xFF04);
    uint8_t current_div_bit = (div >> bit_shift) & 0x1;
    if (current_div_bit == 0 && pre_div_bit == 1) {
        div_apu_cycle = (div_apu_cycle + 1) % 8;
    }
    pre_div_bit = current_div_bit;
}


void APU::queue_sample(float sample) {
    // logger.get_logger()->debug("Queueing sample index of: {:d}", sample_counter);
    if (sample_counter >= 800) return;
    sample_buffer[sample_counter] = sample;
    sample_counter++;
}

void APU::clear_sample_queue() {
    sample_counter = 0;
}

float APU::dac(uint8_t sample) {
    return 1 - ((float) (sample & 0xF))*0.13;
}

Channel1::Channel1(uint8_t *square_wave) {
    this->square_wave = square_wave;
    current_step = 0;
    reset_period_counter();
}

void Channel1::tick(uint8_t div_apu_cycle) {
    if (div_apu_cycle != pre_div_apu) {
        pre_div_apu = div_apu_cycle;
        div_apu_changed = true;
    }
    else div_apu_changed = false;

    uint8_t nr14 = Memory::unsafe_read(0xFF14);
    uint8_t nr12 = Memory::unsafe_read(0xFF12);
    uint8_t trigger = (nr14 >> 7) & 0x1;
    uint8_t length_enable = (nr14 >> 6) & 0x1;
    uint8_t sweep_pace = nr12 & 0x7;
    uint8_t env_direction = nr12 >> 3 & 0x1;
    dac_enabled = nr12 & 0xF8;
    if (!trigger || (length_counter > 0x3F)) {
        // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
        reset_period_counter();
        reset_length_counter();
        current_sample = 0;
        volume = (nr12 >> 4) & 0xF;
        return;
    }
    // TODO: Separate length timer and envelope into individual tick function and let div apu control it
    // Avoid ticking these same speed as channel to avoid having to check if div apu has changed
    if (length_enable && div_apu_changed && div_apu_cycle % 2 == 0) length_counter++;
    if (sweep_pace != 0 && div_apu_changed && div_apu_cycle % 8 == 0) {
        volume_sweep_counter = (volume_sweep_counter+1) % 8;
        if (volume_sweep_counter % sweep_pace == 0) {
            // logger.get_logger()->debug("Volume envelope decrease when sweep_counter: {:X}, sweep pace: {:X}", volume_sweep_counter, sweep_pace);
            // logger.get_logger()->debug("Current volume: {:X}", volume);
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }

    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow -> Sample a step from square wave
        reset_period_counter();
        uint8_t nr11 = (Memory::unsafe_read(0xFF11) >> 6) & 0x3;
        current_sample = ((square_wave[nr11] >> (7-current_step)) & 0x1) * volume;
        current_step = (current_step + 1) % 8;
    }
}

void Channel1::reset_period_counter() {
    uint8_t nr14 = Memory::unsafe_read(0xFF14);
    uint8_t nr13 = Memory::unsafe_read(0xFF13);
    period_counter = (nr14 & 0x7) << 8| nr13;
}

void Channel1::reset_length_counter() {
    uint8_t nr11 = Memory::unsafe_read(0xFF11);
    length_counter = nr11 & 0x3F;
}

Channel2::Channel2(uint8_t *square_wave) {
    this->square_wave = square_wave;
    current_step = 0;
    reset_period_counter();
}

void Channel2::tick(uint8_t div_apu_cycle) {
    uint8_t nr24 = Memory::unsafe_read(0xFF19);
    uint8_t nr22 = Memory::unsafe_read(0xFF17);
    uint8_t trigger = (nr24 >> 7) & 0x1;
    uint8_t length_enable = (nr24 >> 6) & 0x1;
    uint8_t sweep_pace = nr22 & 0x7;
    uint8_t env_direction = nr22 >> 3 & 0x1;
    dac_enabled = nr22 & 0xF8;
    if (!trigger || (length_counter > 0x3F)) {
        // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
        reset_period_counter();
        reset_length_counter();
        current_sample = 0;
        volume = (nr22 >> 4) & 0xF;
        return;
    }
    if (length_enable && div_apu_cycle % 2 == 0) length_counter++;
    if (div_apu_cycle % 8 == 0 && sweep_pace != 0) volume_sweep_counter++;
    if (sweep_pace != 0 && volume_sweep_counter % sweep_pace == 0) {
        volume_sweep_counter = (volume_sweep_counter+1) % 8;
        if (env_direction == 0 && volume > 0) volume -= 1;
        else if (env_direction == 1 && volume < 0xF) volume += 1;
    }

    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow -> Sample a step from square wave
        reset_period_counter();
        uint8_t nr21 = (Memory::unsafe_read(0xFF16) >> 6) & 0x3;
        current_sample = ((square_wave[nr21] >> (7-current_step)) & 0x1) * volume;
        current_step = (current_step + 1) % 8;
    }
}

void Channel2::reset_period_counter() {
    uint8_t nr24 = Memory::unsafe_read(0xFF19);
    uint8_t nr23 = Memory::unsafe_read(0xFF18);
    period_counter = (nr24 & 0x7) << 8| nr23;
}

void Channel2::reset_length_counter() {
    uint8_t nr21 = Memory::unsafe_read(0xFF16);
    length_counter = nr21 & 0x3F;
}
