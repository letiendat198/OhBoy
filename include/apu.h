#ifndef APU_H
#define APU_H
#include <cstdint>
#include <logger.h>
#include <blip_buf.h>
#include <config.h>

template <class T> class AudioChannel { // Common for all audio channels
protected:
    uint8_t current_step = 0;
    uint16_t initial_period_counter = 0;
    uint8_t length_counter = 0;
    uint8_t volume = 1;

    uint8_t current_sample = 0;

    // Only length timer and period are common features. Envelope, volume, sweep are not common at all
    void reset_period_counter();
    void reset_length_counter();
public:
    // Register content
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;

    bool dac_enabled = false;
    bool enabled = false;

    uint8_t get_current_sample();

    void tick();
    void tick_length_timer();
};

class SquareWaveChannel: public AudioChannel<SquareWaveChannel> {
protected:
    uint8_t square_wave[4] = {0b00000001, 0b10000001, 0b10000111, 0b01111110};
    uint8_t channel_number = 1;
    uint8_t volume_sweep_counter = 0;
    uint8_t period_sweep_counter = 0;
    uint16_t shadow_period_register = 0;
    bool sweep_internal_enable = false;
public:
    uint8_t LENGTH_OVERFLOW = 0x3F;

    explicit SquareWaveChannel(uint8_t channel_number): channel_number(channel_number) {}

    void trigger();
    void on_period_overflow();
    void tick_volume_env();
    void tick_period_sweep();
    void check_dac_status();
};

class WaveChannel: public AudioChannel<WaveChannel> {
public:
    uint8_t LENGTH_OVERFLOW = 0xFF;

    void trigger();
    void on_period_overflow();
    void check_dac_status();
};

class NoiseChannel: public AudioChannel<NoiseChannel> {
protected:
    uint8_t volume_sweep_counter = 0;
    uint16_t lfsr = 0;
    uint8_t divider_lookup[8] = {8, 16, 32, 48, 64, 80, 96, 112};
public:
    uint8_t LENGTH_OVERFLOW = 0x3F;

    void trigger();
    void on_period_overflow();
    void tick_lfsr();
    void tick_volume_env();
    void check_dac_status();
};

class APU {
private:
    Logger logger = Logger("APU");
    int cycle = 0;
    int cycle_needed_per_sample = 0;
    uint8_t div_apu_cycle = 0;
    uint8_t pre_div_bit = 0;
public:
    SquareWaveChannel channel1 = SquareWaveChannel(1);
    SquareWaveChannel channel2 = SquareWaveChannel(2);
    WaveChannel channel3 = WaveChannel();
    NoiseChannel channel4 = NoiseChannel();

    bool is_enabled = true;

    blip_buffer_t* blip;
    float* blip_out = new float[SAMPLE_COUNT+100]();
    int sample_count = 0;
    APU();
    void sample();
    void tick_div_apu();

    void clear_sample_queue();
};

#endif //APU_H
