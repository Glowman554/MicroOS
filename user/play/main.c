#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/getc.h>
#include <sys/sound.h>
#include <assert.h>

struct note_t {
	uint16_t length_ms;
	uint16_t note;
};

struct foxm_t {
	uint32_t magic; // 0xf0f0baba
	struct note_t notes[];
};

int main(int argc, char* argv[], char* envp[]) {
	if (argc != 2) {
		printf("Usage: %s <notes file>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	if (!f) {
		printf("Failed to open %s\n", argv[1]);
		return 1;
	}

	fsize(f, file_size);
	void* buffer = malloc(file_size);
	fread(buffer, file_size, 1, f);
	fclose(f);

	struct foxm_t* foxm = buffer;
	assert(foxm->magic == 0xf0f0baba);
	struct note_t* notes = foxm->notes;

	int num_notes = (file_size - sizeof(uint32_t)) / sizeof(struct note_t);

	sound_context_t* context = malloc(sizeof(sound_context_t) + (sizeof(queued_note_t) * (num_notes * 2)));
	memset(context, 0, sizeof(sound_context_t) + (sizeof(queued_note_t) * (num_notes * 2)));

	for (int i = 0; i < num_notes; i++) {
		context->notes[i * 2] = (queued_note_t) {
			.note = notes[i].note,
			.duration_ms = notes[i].length_ms
		};

		context->notes[i * 2 + 1] = (queued_note_t) {
			.note = 0,
			.duration_ms = 200
		};
	}

	free(buffer);

	context->num_notes = num_notes * 2;
	printf("Playing %d notes!\n", context->num_notes);
	printf("Press <esc> to quit!\n");

	coro_t coro = { 0 };
	while (async_getc() != 27) {
		sound_run(&coro, context);
	}

	free(context);

	sound_clear(0);
}