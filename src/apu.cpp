#include "apu.h"
#include <cpu.h>
#include "scheduler.h"

inline void reset_channel_registers(ChannelRegisters *regs) {
    regs->NRx0 = 0;
    regs->NRx1 = 0;
    regs->NRx2 = 0;
    regs->NRx3 = 0;
    regs->NRx4 = 0;
}

SquareWaveChannel::SquareWaveChannel(uint8_t channel_num): logger(Logger("SQUARE_CHANNEL_"+std::to_string(channel_num))) {
    PERIOD_OVERFLOW_EVENT = channel_num == 1 ? SQUARE1_PERIOD_OVERFLOW : SQUARE2_PERIOD_OVERFLOW;
}

void SquareWaveChannel::trigger() {
    is_enabled = is_dac_enabled;
    volume = (reg.NRx2 >> 4) & 0xF;
    if (length_counter > LENGTH_OVERFLOW) length_counter = reg.NRx1 & 0x3F;
    volume_envelope_counter = 0;

    sweep.shadow_reg = ((reg.NRx4 & 0x7) << 8) | reg.NRx3;
    sweep.is_enabled = (sweep_pace != 0) || (sweep_step != 0);

    if (sweep_step != 0) calc_freq_sweep(false);
    sweep.sweep_timer = sweep_pace;

    if (!is_enabled) return;
    uint16_t freq = 0x800 - (((reg.NRx4 & 0x7) << 8) | reg.NRx3);
    Scheduler::reschedule(PERIOD_OVERFLOW_EVENT, freq); // VERY IMPORTANT TO RE-SCHEDULE. OTHERWISE, MULTIPLE OVERFLOW MAKE PITCH THROUGH THE ROOF
}

void SquareWaveChannel::disable() {
    is_enabled = false;
    sample = 0;
    Scheduler::remove_schedule(PERIOD_OVERFLOW_EVENT);
}

void SquareWaveChannel::on_period_overflow() {
    uint8_t duty = (reg.NRx1 >> 6) & 0x3;
    sample = ((square_wave[duty] >> (7-duty_step)) & 0x1) * volume;
    duty_step = (duty_step + 1) & 0x7;

    uint16_t freq = 0x800 - (((reg.NRx4 & 0x7) << 8) | reg.NRx3);
    Scheduler::schedule(PERIOD_OVERFLOW_EVENT, freq);
}

void SquareWaveChannel::tick_length_timer() {
    if (reg.NRx4 >> 6 & 0x1) length_counter++;
    if (length_counter > LENGTH_OVERFLOW) disable();
}

void SquareWaveChannel::tick_volume_envelope() {
    uint8_t sweep_pace = reg.NRx2 & 0x7;
    uint8_t env_direction = reg.NRx2 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_envelope_counter = (volume_envelope_counter+1) % 8;
        if (volume_envelope_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}

void SquareWaveChannel::calc_freq_sweep(bool update_value) {
    uint16_t new_period = 0;
    if (sweep_direction == 0) new_period = sweep.shadow_reg + (sweep.shadow_reg >> sweep_step);
    else new_period = sweep.shadow_reg - (sweep.shadow_reg >> sweep_step);

    if (new_period > 0x7FF) disable();
    else if (sweep_step && update_value) {
        sweep.shadow_reg = new_period;
        reg.NRx3 = new_period & 0xFF;
        reg.NRx4 = (reg.NRx4 & 0xFFF8) | (new_period >> 8 & 0x7);
    }
}


void SquareWaveChannel::tick_freq_sweep() {
    if (!sweep.is_enabled || sweep_pace == 0) return;
    sweep.sweep_timer--;
    if (sweep.sweep_timer == 0) {
        sweep.sweep_timer = sweep_pace;
        calc_freq_sweep(true);
        calc_freq_sweep(false); // Do it again to check for overflow, no write back
    }
}


void WaveChannel::trigger() {
    is_enabled = is_dac_enabled;
    if (length_counter > LENGTH_OVERFLOW) length_counter = reg.NRx1;
    volume = (reg.NRx2 >> 5) & 0x3;
    wave_ram_step = 1;

    if (!is_enabled) return;
    uint16_t freq = 0x400 - (((reg.NRx4 & 0x7) << 8) | reg.NRx3) / 2;
    Scheduler::reschedule(WAVE_PERIOD_OVERFLOW, freq);
}

void WaveChannel::disable() {
    is_enabled = false;
    sample = 0;
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

    uint16_t freq = 0x400 - ((reg.NRx4 & 0x7) << 8 | reg.NRx3) / 2;
    Scheduler::schedule(WAVE_PERIOD_OVERFLOW, freq);
}

void WaveChannel::tick_length_timer() {
    if (reg.NRx4 >> 6 & 0x1) length_counter++;
    if (length_counter > LENGTH_OVERFLOW) disable();
}

void NoiseChannel::trigger() {
    is_enabled = is_dac_enabled;
    volume = (reg.NRx2 >> 4) & 0xF;
    if (length_counter > LENGTH_OVERFLOW) length_counter = reg.NRx1 & 0x3F;
    volume_envelope_counter = 0;
    lfsr = 0;

    if (!is_enabled) return;
    uint16_t freq = divider_lookup[reg.NRx3 & 0x7] << ((reg.NRx3 >> 4) & 0xF);
    // logger.get_logger()->debug("Noise period overflow scheduled at: {:d}", freq);
    Scheduler::reschedule(NOISE_PERIOD_OVERFLOW, freq);
}

void NoiseChannel::disable() {
    is_enabled = false;
    Scheduler::remove_schedule(NOISE_PERIOD_OVERFLOW);
}

void NoiseChannel::on_period_overflow() {
    uint8_t temp = ~((lfsr & 0x1) ^ ((lfsr >> 1) & 0x1));
    lfsr = (lfsr & 0x7FFF) | (temp << 15);
    if (((reg.NRx3 >> 3) & 0x1) == 1) lfsr = (lfsr & 0xFF7F) | (temp << 7);
    lfsr = lfsr >> 1;
    // logger.get_logger()->debug("Current LFSR: {:04X}", lfsr);

    sample = (lfsr & 0x1) * volume;
    uint16_t freq = divider_lookup[reg.NRx3 & 0x7] << ((reg.NRx3 >> 4) & 0xF);
    // logger.get_logger()->debug("Noise period overflow scheduled at: {:d}", freq);
    Scheduler::schedule(NOISE_PERIOD_OVERFLOW, freq);
}

void NoiseChannel::tick_length_timer() {
    if (reg.NRx4 >> 6 & 0x1) length_counter++;
    if (length_counter > LENGTH_OVERFLOW) disable();
}

void NoiseChannel::tick_volume_envelope() {
    uint8_t sweep_pace = reg.NRx2 & 0x7;
    uint8_t env_direction = reg.NRx2 >> 3 & 0x1;
    if (sweep_pace != 0) {
        volume_envelope_counter = (volume_envelope_counter+1) % 8;
        if (volume_envelope_counter % sweep_pace == 0) {
            if (env_direction == 0 && volume > 0) volume -= 1;
            else if (env_direction == 1 && volume < 0xF) volume += 1;
        }
    }
}


// Panning should be 4-bit
inline short mix_sample(uint8_t s1, uint8_t s2, uint8_t s3, uint8_t s4, uint8_t panning) {
    short sample = 0;
    sample += s1 * (panning & 0x1);
    sample += s2 * ((panning >> 1) & 0x1);
    sample += s3 * ((panning >> 2) & 0x1);
    sample += s4 * ((panning >> 3) & 0x1);
    return sample;
}

void APU::sample() {
    uint8_t sample1 = channel1.sample;
    uint8_t sample2 =channel2.sample;
    uint8_t sample3 = channel3.sample;
    uint8_t sample4 = channel4.sample;

    short left_sample = mix_sample(sample1, sample2, sample3, sample4, sound_panning >> 4 & 0xFF);
    short right_sample = mix_sample(sample1, sample2, sample3, sample4, sound_panning & 0xFF);

    uint8_t left_volume = master_volume >> 4 & 0x7;
    uint8_t right_volume = master_volume & 0x7;

    left_volume = left_volume ? left_volume : 1;
    right_volume = right_volume ? right_volume : 1;

    sample_output[sample_count++] = (left_sample * left_volume + right_sample * right_volume) * 10;
    // sample_output[sample_count++] = sample4 * 100;
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
        channel4.tick_length_timer();
    }
    if (div_apu_cycle == 2 || div_apu_cycle == 6) {
        channel1.tick_freq_sweep();
    }
    if (div_apu_cycle == 7) {
        channel1.tick_volume_envelope();
        channel2.tick_volume_envelope();
        channel4.tick_volume_envelope();
    }
}

void APU::disable() {
    is_enabled = false;
    master_volume = 0;
    sound_panning = 0;
    channel1.disable();
    reset_channel_registers(&channel1.reg);
    channel2.disable();
    reset_channel_registers(&channel2.reg);
    channel3.disable();
    reset_channel_registers(&channel3.reg);
    channel4.disable();
    reset_channel_registers(&channel4.reg);
}


void APU::write_apu_register(uint16_t addr, uint8_t data) {
    if (!is_enabled && addr != 0xFF26) return;
   switch (addr) {
        case 0xFF10: { // NR10
            channel1.reg.NRx0 = data;

            uint8_t temp_pace = data >> 4 & 0x7;
            if (temp_pace != 0 && channel1.sweep_pace == 0) channel1.sweep.sweep_timer = temp_pace;
            channel1.sweep_pace = temp_pace;
            channel1.sweep_direction = data >> 3 & 0x1;
            channel1.sweep_step = data & 0x7;
            break;
        }
        case 0xFF11: { // NR11
            channel1.reg.NRx1 = data;
            break;
        }
        case 0xFF12: { // NR12
            channel1.reg.NRx2 = data;
            if ((data >> 3) == 0) {
                channel1.is_dac_enabled = false;
                channel1.disable();
            }
            else channel1.is_dac_enabled = true;
            break;
        }
        case 0xFF13: { // NR13
            channel1.reg.NRx3 = data;
            break;
        }
        case 0xFF14: { // NR14
            channel1.reg.NRx4 = data;
            if ((data >> 7) & 0x1) channel1.trigger();
            break;
        }
        case 0xFF16: { // NR21
            channel2.reg.NRx1 = data;
            break;
        }
        case 0xFF17: { // NR22
            channel2.reg.NRx2 = data;
            if ((data >> 3) == 0) {
                channel2.is_dac_enabled = false;
                channel2.disable();
            }
            else channel2.is_dac_enabled = true;
            break;
        }
        case 0xFF18: { // NR23
            channel2.reg.NRx3 = data;
            break;
        }
        case 0xFF19: { // NR24
            channel2.reg.NRx4 = data;
            if ((data >> 7) & 0x1) channel2.trigger();
            break;
        }
        case 0xFF1A: { // NR30
            channel3.reg.NRx0 = data;
            if ((data >> 7 & 0x1) == 0) {
                channel3.is_dac_enabled = false;
                channel3.disable();
            }
            else channel3.is_dac_enabled = true;
            break;
        }
        case 0xFF1B: { // NR31
            channel3.reg.NRx1 = data;
            break;
        }
        case 0xFF1C: { // NR32
            channel3.reg.NRx2 = data;
            break;
        }
        case 0xFF1D: { // NR33
            channel3.reg.NRx3 = data;
            break;
        }
        case 0xFF1E: { // NR34
            channel3.reg.NRx4 = data;
            if ((data >> 7) & 0x1) channel3.trigger();
            break;
        }
        case 0xFF20: { // NR41
            channel4.reg.NRx1 = data;
            break;
        }
        case 0xFF21: { // NR42
            channel4.reg.NRx2 = data;
            if ((data >> 3) == 0) {
                channel4.is_dac_enabled = false;
                channel4.disable();
            }
            else channel4.is_dac_enabled = true;
            break;
        }
        case 0xFF22: { // NR43
            channel4.reg.NRx3 = data;
            break;
        }
        case 0xFF23: { // NR44
            channel4.reg.NRx4 = data;
            if ((data >> 7) & 0x1) channel4.trigger();
            break;
        }
       case 0xFF24: { // NR50
            master_volume = data;
            break;
        }
       case 0xFF25: { // NR51
            sound_panning = data;
            break;
        }
       case 0xFF26: { // NR52
            is_enabled = data >> 7 & 0x1;
            if (!is_enabled) disable();
            logger.get_logger()->debug("NR52 write: {:X}", data);
            break;
       }
       default:
           logger.get_logger()->debug("Write invalid or unused APU register address: {:#X}", addr);
   }
}

uint8_t APU::read_apu_register(uint16_t addr) {
    uint8_t raw_register_data = 0;
    switch (addr) {
        case 0xFF10: { // NR10
            raw_register_data = channel1.reg.NRx0;
            break;
        }
        case 0xFF11: { // NR11
            raw_register_data = channel1.reg.NRx1;
            break;
        }
        case 0xFF12: { // NR12
            raw_register_data = channel1.reg.NRx2;
            break;
        }
        case 0xFF13: { // NR13
            raw_register_data = channel1.reg.NRx3;
            break;
        }
        case 0xFF14: { // NR14
            raw_register_data = channel1.reg.NRx4;
            break;
        }
        case 0xFF16: { // NR21
            raw_register_data = channel2.reg.NRx1;
            break;
        }
        case 0xFF17: { // NR22
            raw_register_data = channel2.reg.NRx2;
            break;
        }
        case 0xFF18: { // NR23
            raw_register_data = channel2.reg.NRx3;
            break;
        }
        case 0xFF19: { // NR24
            raw_register_data = channel2.reg.NRx4;
            break;
        }
        case 0xFF1A: { // NR30
            raw_register_data = channel3.reg.NRx0;
            break;
        }
        case 0xFF1B: { // NR31
            raw_register_data = channel3.reg.NRx1;
            break;
        }
        case 0xFF1C: { // NR32
            raw_register_data = channel3.reg.NRx2;
            break;
        }
        case 0xFF1D: { // NR33
            raw_register_data = channel3.reg.NRx3;
            break;
        }
        case 0xFF1E: { // NR34
            raw_register_data = channel3.reg.NRx4;
            break;
        }
        case 0xFF20: { // NR41
            raw_register_data = channel4.reg.NRx1;
            break;
        }
        case 0xFF21: { // NR42
            raw_register_data = channel4.reg.NRx2;
            break;
        }
        case 0xFF22: { // NR43
            raw_register_data = channel4.reg.NRx3;
            break;
        }
        case 0xFF23: { // NR44
            raw_register_data = channel4.reg.NRx4;
            break;
        }
        case 0xFF24: { // NR50
            raw_register_data = master_volume;
            break;
        }
        case 0xFF25: { // NR51
            raw_register_data = sound_panning;
            break;
        }
        case 0xFF26: { // NR52
            raw_register_data = is_enabled << 7 | channel4.is_enabled << 3 | channel3.is_enabled << 2 |
                channel2.is_enabled << 1 | channel1.is_enabled;
            logger.get_logger()->debug("NR52 read: {:X}", raw_register_data | IO_READ_VALUE_DMG_CGB[(addr & 0xFF) - 0x10]);
            break;
        }
        default:
            logger.get_logger()->debug("Read invalid or unused APU register address: {:#X}", addr);
    }
    logger.get_logger()->debug("APU register read at addr {:#X}: {:X}", addr, raw_register_data);
    return raw_register_data | IO_READ_VALUE_DMG_CGB[(addr & 0xFF) - 0x10];
}
