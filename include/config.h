//
// Created by ADMIN on 21/10/2024.
//

#ifndef CONFIG_H
#define CONFIG_H

static const int MASTER_CLOCK_FREQ = 4194304;
static const float MS_PER_FRAME = 16.74;
static const float FRAME_RATE = 59.7275;
static uint32_t CYCLE_PER_FRAME  = 17556; // BEWARE THIS COULD BE CHANGE

// APU CONST
static const uint16_t SAMPLE_COUNT = 800;
static const uint32_t OUTPUT_FREQUENCY = 48000;
static const uint16_t CYCLE_PER_SAMPLE = CYCLE_PER_FRAME * 60 / OUTPUT_FREQUENCY;

// TIMER CONST
static const uint16_t DIV_OVERFLOW_CYCLE = 0x4000;

#endif //CONFIG_H
