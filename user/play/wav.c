#include <wav.h>
#include <sys/sound.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bool check_fourcc(const char* data, const char* expected) {
    return data[0] == expected[0] && data[1] == expected[1] && data[2] == expected[2] && data[3] == expected[3];
}

wav_info_t wav_parse_header(uint8_t* buffer, uint32_t buffer_size) {
    wav_info_t info = { .valid = false };

    if (buffer_size < sizeof(wav_riff_header_t) + sizeof(wav_fmt_chunk_t) + sizeof(wav_data_chunk_t)) {
        return info;
    }

    const wav_riff_header_t* riff = (const wav_riff_header_t*) buffer;

    if (!check_fourcc(riff->chunk_id, "RIFF") || !check_fourcc(riff->format, "WAVE")) {
        return info;
    }

    uint32_t offset = sizeof(wav_riff_header_t);
    const wav_fmt_chunk_t* fmt = NULL;
    
    while (offset + 8 < buffer_size) {
        const char* chunk_id = (const char*) (buffer + offset);
        uint32_t chunk_size = *(const uint32_t*) (buffer + offset + 4);
        
        if (check_fourcc(chunk_id, "fmt ")) {
            fmt = (const wav_fmt_chunk_t*) (buffer + offset);
            offset += 8 + chunk_size;
            break;
        }
        offset += 8 + chunk_size;
    }

    if (!fmt) {
        return info;
    }

    if (fmt->audio_format != 1) {
        return info;
    }

    while (offset + 8 < buffer_size) {
        const char* chunk_id = (const char*) (buffer + offset);
        uint32_t chunk_size = *(const uint32_t*) (buffer + offset + 4);
        
        if (check_fourcc(chunk_id, "data")) {
            info.audio_format = fmt->audio_format;
            info.num_channels = fmt->num_channels;
            info.sample_rate = fmt->sample_rate;
            info.bits_per_sample = fmt->bits_per_sample;
            info.data_size = chunk_size;
            info.data_offset = offset + 8;
            info.valid = true;
            return info;
        }
        offset += 8 + chunk_size;
    }

    return info;
}

uint8_t* wav_get_audio_data(uint8_t* buffer, wav_info_t* info) {
    if (!info->valid) {
        return NULL;
    }
    return buffer + info->data_offset;
}

int16_t interpolate_sample(int16_t s1, int16_t s2, uint32_t frac) {
    int32_t diff = (int32_t)s2 - (int32_t)s1;
    return s1 + (int16_t)((diff * frac) >> 16);
}

uint8_t* resample_audio(uint8_t* src_data, const wav_info_t* info, uint32_t target_rate, uint32_t* out_size) {
    if (info->sample_rate == target_rate && info->bits_per_sample == 16) {
        uint8_t* buffer = malloc(info->data_size);
        memcpy(buffer, src_data, info->data_size);
        *out_size = info->data_size;
        return buffer;
    }

    uint32_t bytes_per_sample = info->bits_per_sample / 8;
    uint64_t num_samples = info->data_size / bytes_per_sample / info->num_channels;
    uint64_t out_samples = (num_samples * (uint64_t)target_rate + info->sample_rate - 1) / info->sample_rate;
    
    uint32_t out_bytes = out_samples * 2 * info->num_channels;

    uint8_t* buffer = malloc(out_bytes);
    if (!buffer) {
        return NULL;
    }

    uint64_t step = ((uint64_t)info->sample_rate << 16) / target_rate;
    uint64_t pos = 0;

    uint64_t total_src_samples = num_samples * info->num_channels;
    int16_t* dst = (int16_t*) buffer;

    if (info->bits_per_sample == 16) {
        const int16_t* src = (const int16_t*) src_data;

        for (uint64_t i = 0; i < out_samples; i++) {
            uint64_t src_idx = pos >> 16;
            uint32_t frac = pos & 0xFFFF;

            for (int ch = 0; ch < info->num_channels; ch++) {
                uint64_t idx1 = src_idx * info->num_channels + ch;
                uint64_t idx2 = (src_idx + 1) * info->num_channels + ch;
                
                if (idx1 >= total_src_samples) {
                    idx1 = total_src_samples - info->num_channels + ch;
                }
                if (idx2 >= total_src_samples) {
                    idx2 = idx1;
                }

                *dst++ = interpolate_sample(src[idx1], src[idx2], frac);
            }
            pos += step;
        }
    } else if (info->bits_per_sample == 8) {
        const uint8_t* src = src_data;

        for (uint64_t i = 0; i < out_samples; i++) {
            uint64_t src_idx = pos >> 16;
            uint32_t frac = pos & 0xFFFF;

            for (int ch = 0; ch < info->num_channels; ch++) {
                uint64_t idx1 = src_idx * info->num_channels + ch;
                uint64_t idx2 = (src_idx + 1) * info->num_channels + ch;
                
                if (idx1 >= total_src_samples) {
                    idx1 = total_src_samples - info->num_channels + ch;
                }
                if (idx2 >= total_src_samples) {
                    idx2 = idx1;
                }

                int16_t s1 = ((int16_t)src[idx1] - 128) << 8;
                int16_t s2 = ((int16_t)src[idx2] - 128) << 8;
                *dst++ = interpolate_sample(s1, s2, frac);
            }
            pos += step;
        }
    } else {
        free(buffer);
        return NULL;
    }

    *out_size = out_bytes;
    return buffer;
}

uint8_t* mono_to_stereo(const uint8_t* src_data, uint32_t size, uint32_t* out_size) {
    uint32_t new_size = size * 2;
    uint8_t* buffer = malloc(new_size);

    const int16_t* src = (const int16_t*) src_data;
    int16_t* dst = (int16_t*) buffer;
    uint32_t num_samples = size / 2;

    for (uint32_t i = 0; i < num_samples; i++) {
        dst[i * 2] = src[i];
        dst[i * 2 + 1] = src[i];
    }

    *out_size = new_size;
    return buffer;
}

bool play_wav_buffer(uint8_t* buffer, uint32_t size) {
    uint32_t target_rate = sound_get_sample_rate();

    wav_info_t info = wav_parse_header(buffer, size);
    if (!info.valid) {
        printf("WAV Player: Invalid WAV file format\n");
        return false;
    }

    if (info.audio_format != 1) {
        printf("WAV Player: Only PCM format supported (got %d)\n", info.audio_format);
        return false;
    }

    if (info.bits_per_sample != 8 && info.bits_per_sample != 16) {
        printf("WAV Player: Only 8-bit and 16-bit audio supported (got %d)\n", info.bits_per_sample);
        return false;
    }

    uint8_t* audio_data = wav_get_audio_data(buffer, &info);
    if (!audio_data) {
        printf("WAV Player: Could not get audio data\n");
        return false;
    }

    uint32_t resampled_size = 0;
    uint8_t* resampled_data = resample_audio(audio_data, &info, target_rate, &resampled_size);
    if (!resampled_data) {
        printf("WAV Player: Failed to resample audio\n");
        return false;
    }

    uint8_t* final_data = resampled_data;
    uint32_t final_size = resampled_size;

    if (info.num_channels == 1) {
        uint32_t stereo_size = 0;
        uint8_t* stereo_data = mono_to_stereo(resampled_data, resampled_size, &stereo_size);
        if (stereo_data) {
            free(resampled_data);
            final_data = stereo_data;
            final_size = stereo_size;
        }
    }

    uint32_t written = sound_write_pcm(final_data, final_size);

    free(final_data);

    return written > 0;
}

bool play_wav_file(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("WAV Player: Could not open file %s\n", path);
        return false;
    }

    fsize(file, file_size);
    if (file_size == 0) {
        printf("WAV Player: File is empty\n");
        fclose(file);
        return false;
    }

    uint8_t* buffer = malloc(file_size);
    if (!buffer) {
        printf("WAV Player: Could not allocate memory for file\n");
        fclose(file);
        return false;
    }

    fread(buffer, file_size, 1, file);
    fclose(file);

    bool result = play_wav_buffer(buffer, file_size);

    free(buffer);

    return result;
}
