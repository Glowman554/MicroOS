#include <sys/sound.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>

void sound_run(coro_t* coro, sound_context_t* context) {
	asm volatile("int $0x30" : : "a"(SYS_SOUND_RUN_ID), "b"(coro), "c"(context));
}


void sound_clear(int channel) {
	sound_context_t* context = malloc(sizeof(sound_context_t) + sizeof(queued_note_t));
	memset(context, 0, sizeof(sound_context_t) + sizeof(queued_note_t));
	context->channel = channel;
	context->num_notes = 1;
	coro_t clear_coro = { 0 };
	sound_run(&clear_coro, context);
	free(context);
}