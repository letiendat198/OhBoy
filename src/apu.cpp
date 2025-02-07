#include "apu.h"

#include <cpu.h>
#include <memory.h>

#include "scheduler.h"

APU::APU(){
    blip = blip_new(SAMPLE_COUNT*10);
    blip_set_rates(blip, MASTER_CLOCK_FREQ / 4, OUTPUT_FREQUENCY);
    cycle_needed_per_sample = blip_clocks_needed(blip, SAMPLE_COUNT);
    std::cout<<cycle_needed_per_sample<<"\n";
}

void APU::sample() {
    short sample1 = channel1.is_enabled?channel1.sample:0;
    short sample2 = channel2.is_enabled?channel2.sample:0;
    // short sample3 = channel3.enabled?channel3.get_current_sample():0;
    // uint8_t sample4 = channel4.enabled?channel4.get_current_sample():0;

    short sample = (sample1 + sample2) * 1000;

    sample_output[sample_count++] = sample;

    // logger.get_logger()->debug("Current sample: {:d}", sample);
}

SquareWaveChannel::SquareWaveChannel(uint8_t channel_num) {
    PERIOD_OVERFLOW_EVENT = channel_num == 1 ? SQUARE1_PERIOD_OVERFLOW : SQUARE2_PERIOD_OVERFLOW;
}

void SquareWaveChannel::trigger() {
    is_enabled = is_dac_enabled;
    volume = (NRx2 >> 4) & 0xF;

    uint16_t freq = 0x800 - (((NRx4 & 0x7) << 8) | NRx3);
    Scheduler::reschedule(PERIOD_OVERFLOW_EVENT, freq); // VERY IMPORTANT TO RE-SCHEDULE. OTHERWISE MULTIPLE OVERFLOW MAKE PITCH THROUGH THE ROOF
}

void SquareWaveChannel::on_period_overflow() {
    uint8_t duty = (NRx1 >> 6) & 0x3;
    sample = ((square_wave[duty] >> (7-duty_step)) & 0x1) * volume;
    duty_step = (duty_step + 1) & 0x7;

    uint16_t freq = 0x800 - (((NRx4 & 0x7) << 8) | NRx3);
    Scheduler::schedule(PERIOD_OVERFLOW_EVENT, freq);
}


