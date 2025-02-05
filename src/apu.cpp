#include "apu.h"

#include <cpu.h>
#include <memory.h>

#include "scheduler.h"

APU::APU(){
    blip = blip_new(SAMPLE_COUNT*10);
    blip_set_rates(blip, 2106720, OUTPUT_FREQUENCY);
    cycle_needed_per_sample = blip_clocks_needed(blip, SAMPLE_COUNT);
    std::cout<<cycle_needed_per_sample<<"\n";
}

void APU::sample() {
    uint8_t sample1 = channel1.enabled?channel1.get_current_sample():0;
    uint8_t sample2 = channel2.enabled?channel2.get_current_sample():0;
    uint8_t sample3 = channel3.enabled?channel3.get_current_sample():0;
    // uint8_t sample4 = channel4.enabled?channel4.get_current_sample():0;

    float f1 = 1 - ((float) (sample1 & 0xF))*0.13;
    float f2 = 1 - ((float) (sample2 & 0xF))*0.13;
    float f3 = 1 - ((float) (sample3 & 0xF))*0.13;

    blip_out[sample_count++] = f1 + f2 + f3;
}

void APU::tick_div_apu() {
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

template<class T>
void AudioChannel<T>::tick_length_timer() {
    if (NRx4 >> 6 & 0x1) length_counter++;
    else length_counter = 0;
    if (length_counter > static_cast<T*>(this)->LENGTH_OVERFLOW) enabled = false;
}

template<class T>
void AudioChannel<T>::reset_length_counter() {
    length_counter = NRx1 & static_cast<T*>(this)->LENGTH_OVERFLOW;
}

template<class T>
void AudioChannel<T>::reset_period_counter() {
    initial_period_counter = (NRx4 & 0x7) << 8| NRx3;
}

template<class T>
uint8_t AudioChannel<T>::get_current_sample() {
    return current_sample;
}

void SquareWaveChannel::trigger() {
    uint8_t sweep_pace = (NRx0 >> 4) & 0x7;
    uint8_t sweep_step = NRx0 & 0x7;
    // logger.get_logger()->debug("Channel turned off. Trigger: {:X}, Length timer: {:d}", trigger, length_counter);
    reset_period_counter();
    if(length_counter > LENGTH_OVERFLOW) reset_length_counter();
    shadow_period_register = initial_period_counter;
    period_sweep_counter = 0;
    sweep_internal_enable = sweep_pace != 0 || sweep_step != 0;
    if (sweep_step != 0) tick_period_sweep();
    volume = (NRx2 >> 4) & 0xF;
    volume_sweep_counter = 0;
    enabled = dac_enabled;
    SchedulerEvent period_overflow_event = channel_number == 1 ? SQUARE1_PERIOD_OVERFLOW : SQUARE2_PERIOD_OVERFLOW;
    if (enabled) Scheduler::schedule(period_overflow_event, 0x800 - initial_period_counter);
}

void SquareWaveChannel::on_period_overflow() {
    reset_period_counter();
    uint8_t duty = NRx1 & 0x3;
    current_sample = ((square_wave[duty] >> (7-current_step)) & 0x1) * volume;
    current_step = (current_step + 1) % 8;
    SchedulerEvent period_overflow_event = channel_number == 1 ? SQUARE1_PERIOD_OVERFLOW : SQUARE2_PERIOD_OVERFLOW;
    Scheduler::schedule(period_overflow_event, 0x800 - initial_period_counter);
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
            NRx3 =  shadow_period_register & 0xFF;
            NRx4 = (NRx4 & 0xF8) | ((shadow_period_register >> 8) & 0x7);
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
    reset_period_counter();
    if (length_counter > LENGTH_OVERFLOW) reset_length_counter();
    current_step = 0;
    volume = ((NRx2 >> 5) & 0x3) - 1;
    enabled = dac_enabled;
    if (enabled) Scheduler::schedule(WAVE_PERIOD_OVERFLOW, (0x800 - initial_period_counter) / 2);
}

void WaveChannel::on_period_overflow() {
    reset_period_counter();
    uint16_t wram_addr = 0xFF30;
    uint8_t current_sample_byte = Memory::unsafe_read(wram_addr + (current_step >> 1));
    uint8_t current_nibble = (current_sample_byte >> 4) & 0xF;
    if (current_step % 2 == 1) current_nibble = current_sample_byte & 0xF;
    if (volume != 0xFF) current_sample = current_nibble >> volume;
    else current_sample = 0;
    current_step = (current_step + 1) % 32;
    Scheduler::schedule(WAVE_PERIOD_OVERFLOW, (0x800 - initial_period_counter) / 2);
}

void WaveChannel::check_dac_status() { // Should be same across channels
    dac_enabled = NRx0 >> 7;
    if (!dac_enabled) enabled = false;
}

void NoiseChannel::trigger() {
    lfsr = 0;
    if(length_counter > LENGTH_OVERFLOW) reset_length_counter();
    volume = (NRx2 >> 4) & 0xF;
    volume_sweep_counter = 0;
    enabled = dac_enabled;
}

void NoiseChannel::on_period_overflow() {
    initial_period_counter++;
    uint8_t shift = (NRx3 >> 4) & 0xF;
    uint8_t divider = NRx3 & 0x7;
    if (initial_period_counter > (divider_lookup[divider] << shift)) {
        initial_period_counter = 0;
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


