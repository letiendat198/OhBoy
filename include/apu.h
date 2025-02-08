#ifndef APU_H
#define APU_H
#include <cstdint>
#include <logger.h>
#include <config.h>

enum SchedulerEvent: uint8_t;

class SquareWaveChannel {
public:
    Logger logger;
    uint8_t square_wave[4] = {0b00000001, 0b10000001, 0b10000111, 0b01111110};
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;

    bool is_enabled = false;
    bool is_dac_enabled = true;
    uint8_t sample = 0;
    uint8_t volume = 0;
    uint8_t duty_step = 0;
    uint8_t length_counter = 0;
    uint8_t volume_envelope_counter = 0;

    uint8_t LENGTH_OVERFLOW = 64;

    SchedulerEvent PERIOD_OVERFLOW_EVENT;

    SquareWaveChannel(uint8_t channel_num);
    void trigger();
    void disable();
    void on_period_overflow();
    void tick_length_timer();
    void tick_volume_envelope();
};

class WaveChannel {
public:
    Logger logger = Logger("WAVE_CHANNEL");
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;

    bool is_enabled = false;
    bool is_dac_enabled = false;
    uint8_t sample = 0;
    uint8_t volume = 0;
    uint8_t wave_ram_step = 1;
    uint16_t length_counter = 0;

    uint16_t LENGTH_OVERFLOW = 0xFF;

    void trigger();
    void disable();
    void on_period_overflow();
    void tick_length_timer();
};

class NoiseChannel {
public:
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;

    bool is_enabled = false;
};

class APU {
private:
    Logger logger = Logger("APU");
    uint8_t div_apu_cycle = 0;
public:
    SquareWaveChannel channel1 = SquareWaveChannel(1);
    SquareWaveChannel channel2 = SquareWaveChannel(2);
    WaveChannel channel3 = WaveChannel();
    NoiseChannel channel4 = NoiseChannel();

    bool is_enabled = true;

    short *sample_output = new short[SAMPLE_COUNT];
    int sample_count = 0;

    void sample();
    void schedule_div_apu();
    void on_div_apu_tick();

    // void clear_sample_queue();
};

#endif //APU_H
