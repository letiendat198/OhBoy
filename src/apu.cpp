#include "apu.h"

#include <cpu.h>
#include <memory.h>

APU::APU(){
    blip = blip_new(SAMPLE_COUNT*10);
    blip_set_rates(blip, 2106720, OUTPUT_FREQUENCY);
    cycle_needed_per_sample = blip_clocks_needed(blip, SAMPLE_COUNT);
    std::cout<<cycle_needed_per_sample<<"\n";

    // channel1.set_trigger(&Memory::c1_trigger);
    // channel2.set_trigger(&Memory::c2_trigger);
    // channel3.set_trigger(&Memory::c3_trigger);
    // channel4.set_trigger(&Memory::c4_trigger);
}

void APU::tick() {
    tick_div_apu();
    cycle++;
    if (cycle % 2 == 0) {
        if (cycle % 4 == 0) {
            channel1.tick();
            channel2.tick();
            channel4.tick();
        }
        channel3.tick();

        short sample1 = channel1.is_dac_enabled()?channel1.get_current_sample():0;
        short sample2 = channel2.is_dac_enabled()?channel2.get_current_sample():0;
        short sample3 = channel3.is_dac_enabled()?channel3.get_current_sample():0;
        short sample4 = channel4.is_dac_enabled()?channel4.get_current_sample():0;

        short sample = sample1 + sample2 + sample3 + sample4;

        if (sample != current_sample) {
            blip_add_delta(blip, cycle/2, (sample - current_sample)*100);
            current_sample = sample;
        }
        if (cycle/2 == cycle_needed_per_sample) {
            blip_end_frame(blip, cycle/2);
            logger.get_logger()->debug("Frame ended, sample available: {:d}", blip_samples_avail(blip));
            blip_read_samples(blip, blip_out, SAMPLE_COUNT, 0);
            while(blip_samples_avail(blip)) {
                blip_read_samples(blip, blip_out+SAMPLE_COUNT, blip_samples_avail(blip), 0); // blip_out can hold SAMPLE+100
            }
            cycle = 0;
        }

        uint8_t nr52 = Memory::unsafe_read(0xFF26);
        Memory::unsafe_write(0xFF26, (nr52 & 0x80) | (channel4.is_enabled() << 3 | channel3.is_enabled() << 2 | channel2.is_enabled() << 1 | channel1.is_enabled()));
    }
}

void APU::tick_div_apu() {
    uint8_t bit_shift = 4;//CPU::double_spd_mode?5:4;
    uint8_t div = Memory::read(0xFF04);
    uint8_t current_div_bit = (div >> bit_shift) & 0x1;
    if (current_div_bit == 0 && pre_div_bit == 1) {
        div_apu_cycle = (div_apu_cycle + 1) % 8;
        if (div_apu_cycle % 2 == 0) {
            channel1.tick_length_timer();
            channel2.tick_length_timer();
            channel3.tick_length_timer();
            channel4.tick_length_timer();
        }
        if (div_apu_cycle == 2 || div_apu_cycle == 6) {
            channel1.tick_period_sweep();
        }
        if (div_apu_cycle == 7) {
            channel1.tick_volume_env();
            channel2.tick_volume_env();
            channel4.tick_volume_env();
        }
    }
    pre_div_bit = current_div_bit;
}

template<class T>
void AudioChannel<T>::tick() {
    update_registers_content();
    static_cast<T*>(this)->trigger();
    static_cast<T*>(this)->check_dac_status();
    if (!enabled) return;
    static_cast<T*>(this)->tick_period_timer();
}

template<class T>
void AudioChannel<T>::tick_length_timer() {
    if (NRx4 >> 6 & 0x1) length_counter++;
    else length_counter = 0;
    if (length_counter > static_cast<T*>(this)->LENGTH_OVERFLOW) enabled = false;
}

template<class T>
void AudioChannel<T>::update_registers_content() {
    NRx0 = Memory::unsafe_read(NRx0_addr);
    NRx1 = Memory::unsafe_read(NRx1_addr);
    NRx2 = Memory::unsafe_read(NRx2_addr);
    NRx3 = Memory::unsafe_read(NRx3_addr);
    NRx4 = Memory::unsafe_read(NRx4_addr);
}

template<class T>
void AudioChannel<T>::reset_length_counter() {
    length_counter = NRx1 & static_cast<T*>(this)->LENGTH_OVERFLOW;
}

template<class T>
void AudioChannel<T>::reset_period_counter() {
    period_counter = (NRx4 & 0x7) << 8| NRx3;
}

template<class T>
bool AudioChannel<T>::is_dac_enabled() {
    return dac_enabled;
}

template<class T>
bool AudioChannel<T>::is_enabled() {
    return enabled;
}

template<class T>
uint8_t AudioChannel<T>::get_current_sample() {
    return current_sample;
}

template<class T>
void AudioChannel<T>::set_trigger(bool *trigger_flag) {
    this->trigger_flag = trigger_flag;
}

void SquareWaveChannel::trigger() {
    if (*trigger_flag) {
        uint8_t sweep_pace = (NRx0 >> 4) & 0x7;
        uint8_t sweep_step = NRx0 & 0x7;
        // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
        reset_period_counter();
        if(length_counter > LENGTH_OVERFLOW) reset_length_counter();
        shadow_period_register = period_counter;
        period_sweep_counter = 0;
        sweep_internal_enable = sweep_pace != 0 || sweep_step != 0;
        if (sweep_step != 0) tick_period_sweep();
        volume = (NRx2 >> 4) & 0xF;
        volume_sweep_counter = 0;
        enabled = dac_enabled;
        *trigger_flag = false;
    }
}

void SquareWaveChannel::tick_period_timer() {
    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow -> Sample a step from square wave
        reset_period_counter();
        uint8_t duty = NRx1 & 0x3;
        current_sample = ((square_wave[duty] >> (7-current_step)) & 0x1) * volume;
        current_step = (current_step + 1) % 8;
    }
}

void SquareWaveChannel::tick_volume_env() {
    uint8_t sweep_pace = NRx2 & 0x7;
    uint8_t env_direction = NRx2 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_sweep_counter = (volume_sweep_counter+1) % 8;
        if (volume_sweep_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}

void SquareWaveChannel::tick_period_sweep() {
    uint8_t pace = (NRx0 >> 4) & 0x7;
    uint8_t direction = NRx0 >> 3 & 0x1;
    uint8_t step = NRx0 & 0x7;
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
            Memory::unsafe_write(NRx3_addr, shadow_period_register & 0xFF);
            Memory::unsafe_write(NRx4_addr, (Memory::unsafe_read(NRx4_addr) & 0xF8) | ((shadow_period_register >> 8) & 0x7));
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

void SquareWaveChannel::check_dac_status() { // Should be same across channels
    dac_enabled = NRx2 & 0xF8;
    if (!dac_enabled) enabled = false;
}

void WaveChannel::trigger() {
    if (*trigger_flag) {
        reset_period_counter();
        if (length_counter > LENGTH_OVERFLOW) reset_length_counter();
        current_step = 0;
        volume = ((NRx2 >> 5) & 0x3) - 1;
        enabled = dac_enabled;
        *trigger_flag = false;
    }
}

void WaveChannel::tick_period_timer() {
    period_counter++;
    if (period_counter > 0x7FF) { // When period counter overflow
        reset_period_counter();
        uint16_t wram_addr = 0xFF30;
        uint8_t current_sample_byte = Memory::unsafe_read(wram_addr + (current_step >> 1));
        uint8_t current_nibble = (current_sample_byte >> 4) & 0xF;
        if (current_step % 2 == 1) current_nibble = current_sample_byte & 0xF;
        if (volume != 0xFF) current_sample = current_nibble >> volume;
        else current_sample = 0;
        current_step = (current_step + 1) % 32;
    }
}

void WaveChannel::check_dac_status() { // Should be same across channels
    dac_enabled = NRx0 >> 7;
    if (!dac_enabled) enabled = false;
}

void NoiseChannel::trigger() {
    if (*trigger_flag) {
        lfsr = 0;
        if(length_counter > LENGTH_OVERFLOW) reset_length_counter();
        volume = (NRx2 >> 4) & 0xF;
        volume_sweep_counter = 0;
        enabled = dac_enabled;
        *trigger_flag = false;
    }
}

void NoiseChannel::tick_period_timer() {
    period_counter++;
    uint8_t shift = (NRx3 >> 4) & 0xF;
    uint8_t divider = NRx3 & 0x7;
    if (period_counter > (divider_lookup[divider] << shift)) {
        period_counter = 0;
        tick_lfsr();
        current_sample = (lfsr & 0x1) * volume;
    }
}

void NoiseChannel::tick_lfsr() {
    uint8_t lfsr_width = (NRx3 >> 3) & 0x1;
    uint8_t temp = ~((lfsr & 0x1) ^ ((lfsr >> 1) & 0x1));
    lfsr = lfsr & 0x7FFF | (temp << 15);
    if (lfsr_width == 1) lfsr = (lfsr & 0xFF7F) | (temp << 7);
    lfsr = lfsr >> 1;
}


void NoiseChannel::tick_volume_env() {
    uint8_t sweep_pace = NRx2 & 0x7;
    uint8_t env_direction = NRx2 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_sweep_counter = (volume_sweep_counter+1) % 8;
        if (volume_sweep_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}

void NoiseChannel::check_dac_status() {
    dac_enabled = NRx2 & 0xF8;
    if (!dac_enabled) enabled = false;
}


