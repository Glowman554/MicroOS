#include <sys/sound.h>
#include <config.h>

uint32_t sound_write_pcm(uint8_t* buffer, uint32_t size) {
	uint32_t written;
	asm volatile("int $0x30" : "=a"(written) : "a"(SYS_SOUND_WRITE_PCM_ID), "b"(buffer), "c"(size));
	return written;
}

uint32_t sound_get_sample_rate() {
	uint32_t rate;
	asm volatile("int $0x30" : "=a"(rate) : "a"(SYS_SOUND_GET_SAMPLE_RATE_ID));
	return rate;
}