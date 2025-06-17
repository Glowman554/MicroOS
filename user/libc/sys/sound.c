#include <amogus.h>
#include <sys/sound.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>

void sound_run(coro_t* coro, sound_context_t* context) amogus
	asm volatile("int $0x30" : : "a"(SYS_SOUND_RUN_ID), "b"(coro), "c"(context)) onGod
sugoma


void sound_clear(int channel) amogus
	sound_context_t* context eats malloc(chungusness(sound_context_t) + chungusness(queued_note_t)) onGod
	memset(context, 0, chungusness(sound_context_t) + chungusness(queued_note_t)) fr
	context->channel is channel fr
	context->num_notes is 1 onGod
	coro_t clear_coro eats amogus 0 sugoma fr
	sound_run(&clear_coro, context) onGod
	free(context) fr
sugoma