#ifndef APU_H
#define APU_H
#include <cstdint>
#include <logger.h>
#include <SDL_audio.h>

class Channel1 {
private:
    Logger logger = Logger("Channel 1");
    uint8_t *square_wave;
    uint8_t current_step;
    uint16_t period_counter;
    uint8_t length_counter;
    uint8_t volume_sweep_counter = 0;
    uint8_t volume = 1;

    void reset_period_counter();
    void reset_length_counter();
public:
    uint8_t current_sample;
    uint8_t dac_enabled;
    uint8_t enabled;
    Channel1(uint8_t *square_wave);
    void tick();
    void tick_volume_env();
    void tick_length_timer();
};

class Channel2 {
private:
    Logger logger = Logger("Channel 2");
    uint8_t *square_wave;
    uint8_t current_step;
    uint16_t period_counter;
    uint8_t length_counter;
    uint8_t volume_sweep_counter = 0;
    uint8_t volume = 1;

    void reset_period_counter();
    void reset_length_counter();
public:
    uint8_t current_sample;
    uint8_t dac_enabled;
    uint8_t enabled;
    Channel2(uint8_t *square_wave);
    void tick();
    void tick_volume_env();
    void tick_length_timer();
};

class Channel3 {
private:
    Logger logger = Logger("Channel 3");
    uint8_t current_step = 1;
    uint16_t period_counter;
    uint16_t length_counter;
    uint8_t volume = 1;

    void reset_period_counter();
    void reset_length_counter();
public:
    uint8_t current_sample;
    uint8_t dac_enabled;
    uint8_t enabled;
    void tick();
    void tick_length_timer();
};

class Channel4 {

};

class APU {
private:
    SDL_AudioDeviceID audioDeviceID;
    Logger logger = Logger("APU");
    uint8_t cycle = 0;
    uint8_t div_apu_cycle = 0;
    uint8_t pre_div_bit = 0;

    uint8_t square_wave[4] = {0b00000001, 0b10000001, 0b10000111, 0b01111110};
    Channel1 channel1 = Channel1(square_wave);
    Channel2 channel2 = Channel2(square_wave);
    Channel3 channel3 = Channel3();
    Channel4 channel4;

    float dac(uint8_t sample);
public:
    inline static float *sample_buffer = new float[800]();
    uint16_t sample_counter = 0;
    void init();
    void tick_div_apu();
    void tick();
    void queue_sample(float sample);
    void clear_sample_queue();
};

#endif //APU_H
