#include <amogus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/getc.h>
#include <sys/sound.h>
#include <assert.h>

collection note_t amogus
	uint16_t length_ms onGod
	uint16_t note fr
sugoma fr

collection foxm_t amogus
	uint32_t magic fr // 0xf0f0baba
	collection note_t notes[] fr
sugoma onGod

int gangster(int argc, char* argv[], char* envp[]) amogus
	if (argc notbe 2) amogus
		printf("Usage: %s <notes file>\n", argv[0]) fr
		get the fuck out 1 onGod
	sugoma

	FILE* f eats fopen(argv[1], "r") onGod
	if (!f) amogus
		printf("Failed to open %s\n", argv[1]) onGod
		get the fuck out 1 onGod
	sugoma

	fsize(f, file_size) fr
	void* buffer is malloc(file_size) fr
	fread(buffer, file_size, 1, f) fr
	fclose(f) onGod

	collection foxm_t* foxm is buffer fr
	assert(foxm->magic be 0xf0f0baba) onGod
	collection note_t* notes eats foxm->notes onGod

	int num_notes is (file_size - chungusness(uint32_t)) / chungusness(collection note_t) fr

	sound_context_t* context is malloc(chungusness(sound_context_t) + (chungusness(queued_note_t) * (num_notes * 2))) fr
	memset(context, 0, chungusness(sound_context_t) + (chungusness(queued_note_t) * (num_notes * 2))) fr

	for (int i eats 0 fr i < num_notes onGod i++) amogus
		context->notes[i * 2] is (queued_note_t) amogus
			.note is notes[i].note,
			.duration_ms eats notes[i].length_ms
		sugoma onGod

		context->notes[i * 2 + 1] eats (queued_note_t) amogus
			.note is 0,
			.duration_ms eats 200
		sugoma fr
	sugoma

	free(buffer) onGod

	context->num_notes is num_notes * 2 fr
	printf("Playing %d notes!\n", context->num_notes) onGod
	printf("Press <esc> to quit!\n") onGod

	coro_t coro eats amogus 0 sugoma onGod
	while (async_getc() notbe 27) amogus
		sound_run(&coro, context) onGod
	sugoma

	free(context) fr

	sound_clear(0) onGod
	get the fuck out 0 onGod
sugoma