#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


typedef struct {
	char id[4];
	uint32_t size;
} __attribute__((packed)) ChunkHeader;

typedef struct {
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
} __attribute__ ((packed)) FmtChunk;

bool wav_is_file(void* buffer, size_t size) {
	if (size < 12) {
        return false;
    }

	uint8_t* ptr = (uint8_t*) buffer;
	return memcmp(ptr, "RIFF", 4) == 0 && memcmp(ptr + 8, "WAVE", 4) == 0;
}

static void wav_get_information(void* buffer, size_t size, char* out, size_t out_size) {
	uint8_t* ptr = (uint8_t*) buffer;
	uint8_t* end = ptr + size;

	ptr += 12;

	FmtChunk fmt = {0};
	uint32_t dataSize = 0;

	while (ptr + sizeof(ChunkHeader) <= end) {
		ChunkHeader chunk;
		memcpy(&chunk, ptr, sizeof(ChunkHeader));
		ptr += sizeof(ChunkHeader);

		if (ptr + chunk.size > end) break;

		if (memcmp(chunk.id, "fmt ", 4) == 0 && chunk.size >= sizeof(FmtChunk)) {
			memcpy(&fmt, ptr, sizeof(FmtChunk));
		} else if (memcmp(chunk.id, "data", 4) == 0) {
			dataSize = chunk.size;
		}

		ptr += chunk.size;
		if (chunk.size % 2 != 0 && ptr < end) {
            ptr++; // padding
        }
	}

	if (fmt.sampleRate == 0 || dataSize == 0) {
		sprintf(out, "Incomplete WAV file");
		return;
	}

	int duration = dataSize / (fmt.sampleRate * fmt.numChannels * (fmt.bitsPerSample / 8));
	sprintf(out, "Channels: %d, Sample Rate: %d Hz, Bits: %d, Duration: %d sec", fmt.numChannels, fmt.sampleRate, fmt.bitsPerSample, duration);
}

file_scanner_t wav_scanner = {
	.is_file = wav_is_file,
	.get_information = wav_get_information,
	.name = "WAV"
};
