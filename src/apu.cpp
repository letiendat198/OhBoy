#include "apu.h"

#include <cpu.h>
#include <memory.h>
#include <config.h>
#include <debugger.h>

void APU::init(){
    blip = blip_new(SAMPLE_COUNT);
    blip_set_rates(blip, 2*CYCLE_PER_FRAME, OUTPUT_FREQUENCY);
    cycle_needed_per_sample = blip_clocks_needed(blip, SAMPLE_COUNT);
    std::cout<<cycle_needed_per_sample<<"\n";
}

void APU::tick() {
    tick_div_apu();
    cycle++;
    if (cycle % 2 == 0) {
        if (cycle % 4 == 0) {
            channel1.tick();
            channel2.tick();
        }
        channel3.tick();
    }

    uint8_t nr52 = Memory::unsafe_read(0xFF26);
    Memory::unsafe_write(0xFF26, (nr52 & 0x80) | (0 << 3 | channel3.enabled << 2 | channel2.enabled << 1 | channel1.enabled));

    if (cycle == 87) {
        float sample_channel1 = channel1.dac_enabled?dac(channel1.current_sample):0;
        float sample_channel2 = channel2.dac_enabled?dac(channel2.current_sample):0;
        float sample_channel3 = channel3.dac_enabled?dac(channel3.current_sample):0;
        // logger.get_logger()->debug("Sample channel 3: {:X}", channel3.current_sample);
        float sample = sample_channel1 + sample_channel2 + sample_channel3;
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
        if (div_apu_cycle % 2 == 0) {
            channel1.tick_length_timer();
            channel2.tick_length_timer();
            channel3.tick_length_timer();
        }
        if (div_apu_cycle == 2 || div_apu_cycle == 6) {
            channel1.tick_period_sweep();
        }
        if (div_apu_cycle == 7) {
            channel1.tick_volume_env();
            channel2.tick_volume_env();
        }
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
    reset_length_counter();
    current_sample = 0;
    uint8_t nr12 = Memory::unsafe_read(0xFF12);
    volume = (nr12 >> 4) & 0xF;
}

void Channel1::tick() {
    uint8_t nr12 = Memory::unsafe_read(0xFF12);
    uint8_t nr10 = Memory::unsafe_read(0xFF10);
    uint8_t sweep_pace = (nr10 >> 4) & 0x7;
    uint8_t sweep_step = nr10 & 0x7;
    dac_enabled = nr12 & 0xF8;
    if (!dac_enabled) enabled = false;
    if (length_counter > 0x3F) enabled = false;
    if (Memory::c1_trigger) {
        // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
        reset_period_counter();
        if(length_counter > 0x3F) reset_length_counter();
        shadow_period_register = period_counter;
        period_sweep_counter = 0;
        sweep_internal_enable = sweep_pace != 0 || sweep_step != 0;
        if (sweep_step != 0) tick_period_sweep();
        current_sample = 0;
        volume = (nr12 >> 4) & 0xF;
        volume_sweep_counter = 0;
        enabled = dac_enabled;
        Memory::c1_trigger = false;
    }
    if (!enabled) return;

    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow -> Sample a step from square wave
        reset_period_counter();
        uint8_t nr11 = (Memory::unsafe_read(0xFF11) >> 6) & 0x3;
        current_sample = ((square_wave[nr11] >> (7-current_step)) & 0x1) * volume;
        current_step = (current_step + 1) % 8;
    }
}

void Channel1::tick_volume_env() {
    uint8_t nr12 = Memory::unsafe_read(0xFF12);
    uint8_t sweep_pace = nr12 & 0x7;
    uint8_t env_direction = nr12 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_sweep_counter = (volume_sweep_counter+1) % 8;
        if (volume_sweep_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}

void Channel1::tick_length_timer() {
    uint8_t nr14 = Memory::unsafe_read(0xFF14);
    uint8_t length_enable = (nr14 >> 6) & 0x1;
    if (length_enable) length_counter++;
}

void Channel1::tick_period_sweep() {
    uint8_t nr10 = Memory::unsafe_read(0xFF10);
    uint8_t pace = (nr10 >> 4) & 0x7;
    uint8_t direction = nr10 >> 3 & 0x1;
    uint8_t step = nr10 & 0x7;
    if (sweep_internal_enable && pace != 0) {
        period_sweep_counter = (period_sweep_counter + 1) % 8;
        if (period_sweep_counter % pace == 0) {
            uint16_t temp_period;
            if (direction == 0) temp_period = shadow_period_register + (shadow_period_register >> step);
            else temp_period = shadow_period_register - (shadow_period_register >> step);
            if (temp_period > 0x7FF) {
                enabled = false;
                return;
            }
            shadow_period_register = temp_period;
            Memory::unsafe_write(0xFF13, shadow_period_register & 0xFF);
            Memory::unsafe_write(0xFF14, (Memory::unsafe_read(0xFF14) & 0xF8) | ((shadow_period_register >> 8) & 0x7));
            // Calculate again???
            if (direction == 0) temp_period = shadow_period_register + (shadow_period_register >> step);
            else temp_period = shadow_period_register - (shadow_period_register >> step);
            if (temp_period > 0x7FF) {
                enabled = false;
                return;
            }
            shadow_period_register = temp_period;
        }
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

void Channel2::tick() {
    uint8_t nr22 = Memory::unsafe_read(0xFF17);
    dac_enabled = nr22 & 0xF8;
    if (!dac_enabled) enabled = false;
    if (length_counter > 0x3F) enabled = false;
    if (Memory::c2_trigger) {
        // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
        reset_period_counter();
        if(length_counter > 0x3F) reset_length_counter();
        current_sample = 0;
        volume = (nr22 >> 4) & 0xF;
        volume_sweep_counter = 0;
        enabled = dac_enabled;
        Memory::c2_trigger = false;
    }
    if (!enabled) return;

    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow -> Sample a step from square wave
        reset_period_counter();
        uint8_t nr21 = (Memory::unsafe_read(0xFF16) >> 6) & 0x3;
        current_sample = ((square_wave[nr21] >> (7-current_step)) & 0x1) * volume;
        current_step = (current_step + 1) % 8;
    }
}

void Channel2::tick_volume_env() {
    uint8_t nr22 = Memory::unsafe_read(0xFF17);
    uint8_t sweep_pace = nr22 & 0x7;
    uint8_t env_direction = nr22 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_sweep_counter = (volume_sweep_counter+1) % 8;
        if (volume_sweep_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}

void Channel2::tick_length_timer() {
    uint8_t nr24 = Memory::unsafe_read(0xFF19);
    uint8_t length_enable = (nr24 >> 6) & 0x1;
    if (length_enable) length_counter++;
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

void Channel3::tick() {
    uint8_t nr32 = Memory::unsafe_read(0xFF1C);
    dac_enabled = Memory::unsafe_read(0xFF1A) >> 7 & 0x1;
    if (!dac_enabled) enabled = false;
    if (length_counter > 0xFF) enabled = false;
    if (Memory::c3_trigger) {
        // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
        reset_period_counter();
        if (length_counter > 0xFF) reset_length_counter();
        current_step = 1;
        volume = ((nr32 >> 5) & 0x3) - 1;
        enabled = dac_enabled;
        Memory::c3_trigger = false;
    }
    if (!enabled) return;

    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow
        reset_period_counter();
        uint16_t wram_addr = 0xFF30;
        uint8_t current_sample_byte = Memory::unsafe_read(wram_addr + (current_step >> 1));
        uint8_t current_nibble = (current_sample_byte >> 4) & 0xF;
        if (current_step % 2 == 1) current_nibble = current_sample_byte & 0xF;
        // logger.get_logger()->debug("Current step: {:d}. Current WRAM addr: {:#X}, Current WRAM data: {:#X}, Current Nibble: {:X}",
        //     current_step, wram_addr + (current_step >> 1),
        //     Memory::unsafe_read(wram_addr + (current_step >> 1)),
        //     current_nibble);
        if (volume != 0xFF) current_sample = current_nibble >> volume;
        else current_sample = 0;
        current_step = (current_step + 1) % 32;
    }
}

void Channel3::tick_length_timer() {
    uint8_t nr34 = Memory::unsafe_read(0xFF1E);
    uint8_t length_enable = (nr34 >> 6) & 0x1;
    if (length_enable) length_counter++;
}

void Channel3::reset_period_counter() {
    uint8_t nr34 = Memory::unsafe_read(0xFF1E);
    uint8_t nr33 = Memory::unsafe_read(0xFF1D);
    period_counter = (nr34 & 0x7) << 8| nr33;
}

void Channel3::reset_length_counter() {
    length_counter = Memory::unsafe_read(0xFF1B);
}