#ifndef APU_H
#define APU_H
#include <cstdint>
#include <logger.h>
#include <blip_buf.h>
#include <config.h>

enum SchedulerEvent: uint8_t;

class SquareWaveChannel {
public:
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

    SchedulerEvent PERIOD_OVERFLOW_EVENT;

    SquareWaveChannel(uint8_t channel_num);
    void trigger();
    void disable();
    void schedule_next_period_overflow();
    void on_period_overflow();
};

class WaveChannel {
public:
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;

    bool is_enabled = false;
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
    short *sample_output = new short[SAMPLE_COUNT];
    int sample_count = 0;

    APU();
    void sample();
    // void tick_div_apu();

    // void clear_sample_queue();
};

#endif //APU_H
