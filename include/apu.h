#ifndef APU_H
#define APU_H
#include <cstdint>
#include <logger.h>
#include <blip_buf.h>
#include <config.h>

template <class T> class AudioChannel {
protected:
    // Register content
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;

    uint16_t NRx0_addr = 0;
    uint16_t NRx1_addr = 0;
    uint16_t NRx2_addr = 0;
    uint16_t NRx3_addr = 0;
    uint16_t NRx4_addr = 0;

    uint8_t current_step = 0;
    uint16_t period_counter = 0;
    uint8_t length_counter = 0;
    uint8_t volume = 1;

    uint8_t current_sample = 0;

    bool dac_enabled = false;
    bool enabled = false;

    bool *trigger_flag = nullptr;

    // Only length timer and period are common features. Envelope, volume, sweep are not common at all
    void reset_period_counter();
    void reset_length_counter();

    void update_registers_content();
public:
    AudioChannel(uint16_t NRx0_addr, uint16_t NRx1_addr, uint16_t NRx2_addr, uint16_t NRx3_addr, uint16_t NRx4_addr):
    NRx0_addr(NRx0_addr), NRx1_addr(NRx1_addr), NRx2_addr(NRx2_addr), NRx3_addr(NRx3_addr), NRx4_addr(NRx4_addr) {};

    void set_trigger(bool *trigger_flag);

    bool is_enabled();
    bool is_dac_enabled();
    uint8_t get_current_sample();

    void tick();
    void tick_length_timer();
};

class SquareWaveChannel: public AudioChannel<SquareWaveChannel> {
protected:
    uint8_t *square_wave;
    uint8_t volume_sweep_counter = 0;
    uint8_t period_sweep_counter = 0;
    uint16_t shadow_period_register = 0;
    bool sweep_internal_enable = false;
public:
    SquareWaveChannel(uint8_t *square_wave, uint16_t NRx0_addr, uint16_t NRx1_addr, uint16_t NRx2_addr, uint16_t NRx3_addr, uint16_t NRx4_addr):
    AudioChannel<SquareWaveChannel>(NRx0_addr, NRx1_addr, NRx2_addr, NRx3_addr, NRx4_addr), square_wave(square_wave) {};

    uint8_t LENGTH_OVERFLOW = 0x3F;

    void trigger();
    void tick_period_timer();
    void tick_volume_env();
    void tick_period_sweep();
    void check_dac_status();
};

class WaveChannel: public AudioChannel<WaveChannel> {
public:
    WaveChannel(uint16_t NRx0_addr, uint16_t NRx1_addr, uint16_t NRx2_addr, uint16_t NRx3_addr, uint16_t NRx4_addr):
    AudioChannel<WaveChannel>(NRx0_addr, NRx1_addr, NRx2_addr, NRx3_addr, NRx4_addr) {};

    uint8_t LENGTH_OVERFLOW = 0xFF;

    void trigger();
    void tick_period_timer();
    void check_dac_status();
};

class NoiseChannel: public AudioChannel<NoiseChannel> {
protected:
    uint8_t volume_sweep_counter = 0;
    uint16_t lfsr = 0;
    uint8_t divider_lookup[8] = {8, 16, 32, 48, 64, 80, 96, 112};
public:
    NoiseChannel(uint16_t NRx0_addr, uint16_t NRx1_addr, uint16_t NRx2_addr, uint16_t NRx3_addr, uint16_t NRx4_addr):
    AudioChannel<NoiseChannel>(NRx0_addr, NRx1_addr, NRx2_addr, NRx3_addr, NRx4_addr) {};

    uint8_t LENGTH_OVERFLOW = 0x3F;

    void trigger();
    void tick_period_timer();
    void tick_lfsr();
    void tick_volume_env();
    void check_dac_status();
};

class APU {
private:
    Logger logger = Logger("APU");
    int cycle = 0;
    int cycle_needed_per_sample = 0;
    short current_sample = 0;
    uint8_t div_apu_cycle = 0;
    uint8_t pre_div_bit = 0;

    uint8_t square_wave[4] = {0b00000001, 0b10000001, 0b10000111, 0b01111110};
    SquareWaveChannel channel1 = SquareWaveChannel(square_wave, 0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14);
    SquareWaveChannel channel2 = SquareWaveChannel(square_wave, 0, 0xFF16, 0xFF17, 0xFF18, 0xFF19);
    WaveChannel channel3 = WaveChannel(0xFF1A, 0xFF1B, 0xFF1C, 0xFF1D, 0xFF1E);
    NoiseChannel channel4 = NoiseChannel(0, 0xFF20, 0xFF21, 0xFF22, 0xFF23);
public:
    blip_buffer_t* blip;
    short* blip_out = new short[SAMPLE_COUNT+100]();
    APU();
    void tick_div_apu();
    void tick();
    void clear_sample_queue();
};

#endif //APU_H
