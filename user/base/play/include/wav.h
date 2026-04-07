#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct wav_riff_header {
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
} __attribute__((packed)) wav_riff_header_t;

typedef struct wav_fmt_chunk {
    char chunk_id[4];
    uint32_t chunk_size;  
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} __attribute__((packed)) wav_fmt_chunk_t;

typedef struct wav_data_chunk {
    char chunk_id[4];
    uint32_t chunk_size;
} __attribute__((packed)) wav_data_chunk_t;

typedef struct wav_info {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint32_t data_size;
    uint32_t data_offset;
    bool valid;
} wav_info_t;

wav_info_t wav_parse_header(uint8_t* buffer, uint32_t buffer_size);
uint8_t* wav_get_audio_data(uint8_t* buffer, wav_info_t* info);
bool play_wav_file(char* path);
bool play_wav_buffer(uint8_t* buffer, uint32_t size);
