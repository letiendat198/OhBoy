#ifndef APU_H
#define APU_H
#include <cstdint>
#include <config.h>

enum EVENT_ID: uint8_t;

struct ChannelRegisters {
    uint8_t NRx0 = 0;
    uint8_t NRx1 = 0;
    uint8_t NRx2 = 0;
    uint8_t NRx3 = 0;
    uint8_t NRx4 = 0;
};

struct SweepUnit {
    bool is_enabled = false;
    uint8_t sweep_timer = 0;
    uint16_t shadow_reg = 0;
};

void reset_channel_registers(ChannelRegisters *regs);

class SquareWaveChannel {
public:
    uint8_t square_wave[4] = {0b00000001, 0b10000001, 0b10000111, 0b01111110};
    ChannelRegisters reg;
    SweepUnit sweep;

    bool is_enabled = false;
    bool is_dac_enabled = true;
    uint8_t sample = 0;
    uint8_t volume = 0;
    uint8_t duty_step = 0;
    uint8_t length_counter = 0;
    uint8_t volume_envelope_counter = 0;

    uint8_t sweep_pace = 0;
    uint8_t sweep_direction = 0;
    uint8_t sweep_step = 0;

    uint8_t LENGTH_OVERFLOW = 64;

    EVENT_ID PERIOD_OVERFLOW_EVENT;

    SquareWaveChannel(uint8_t channel_num);
    void trigger();
    void disable();
    void on_period_overflow();
    void tick_length_timer();
    void tick_volume_envelope();
    void tick_freq_sweep();
    void calc_freq_sweep(bool update_value);
};

class WaveChannel {
public:
    ChannelRegisters reg;

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
    ChannelRegisters reg;

    const uint8_t divider_lookup[8] = {2, 4, 8, 12, 16, 20, 24, 28};

    bool is_enabled = false;
    bool is_dac_enabled = false;
    uint8_t sample = 0;
    uint16_t lfsr = 0;
    uint8_t volume = 0;
    uint8_t length_counter = 0;
    uint8_t volume_envelope_counter = 0;

    uint8_t LENGTH_OVERFLOW = 64;

    void trigger();
    void disable();
    void on_period_overflow();
    void tick_length_timer();
    void tick_volume_envelope();
};

class APU {
private:
    uint8_t div_apu_cycle = 0;

    inline static const uint8_t IO_READ_VALUE_DMG_CGB[32] = { // READ MASK
        0x80, // NR10
        0x3F, // NR11
        0x00, // NR12
        0xFF, // NR13
        0xBF, // NR14
        0xFF,
        0x3F, // NR21
        0x00, // NR22
        0xFF, // NR23
        0xBF, // NR24
        0x7F, // NR30
        0xFF, // NR31
        0x9F, // NR32
        0xFF, // NR33
        0xBF, // NR34
        0xFF,
        0xFF, // NR41
        0x00, // NR42
        0x00, // NR43
        0xBF, // NR44
        0x00, // NR50
        0x00, // NR51
        0x70, // NR52
        0xFF, // 0x27
        0xFF, // 0x28
        0xFF, // 0x29
        0xFF, // 0x2A
        0xFF, // 0x2B
        0xFF, // 0x2C
        0xFF, // 0x2D
        0xFF, // 0x2E
        0xFF // 0x2F
    };
public:
    SquareWaveChannel channel1 = SquareWaveChannel(1);
    SquareWaveChannel channel2 = SquareWaveChannel(2);
    WaveChannel channel3 = WaveChannel();
    NoiseChannel channel4 = NoiseChannel();

    bool is_enabled = true;
    uint8_t sound_panning = 0xFF;
    uint8_t master_volume = 0xFF;

    short *sample_output = new short[SAMPLE_COUNT];
    int sample_count = 0;

    void sample();
    void schedule_div_apu();
    void on_div_apu_tick();
    void disable();

    void write_apu_register(uint16_t addr, uint8_t data);
    uint8_t read_apu_register(uint16_t addr);
};

#endif //APU_H
