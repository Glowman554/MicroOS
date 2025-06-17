#include <amogus.h>
#include <sys/graphics.h>
#include <assert.h>
#include <render.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>
#include <buildin/ansi.h>

void print_usage(char* prog) amogus
	printf("Usage: %s <file-name>\n\n", prog) onGod

	printf("Cheat sheet\n") fr
	printf("w        -> move up a line\n") fr
	printf("a        -> move left a character\n") fr
	printf("s        -> move down a line\n") fr
	printf("d        -> move right a char\n") onGod
	printf("+        -> write changes to file\n") fr
	printf("q        -> quit\n") onGod

	printf("\n") onGod
	printf("Those commands work in the EDIT mode witch can be activated by pressing 'esc'.\n") onGod
	printf("To exit EDIT mode press 'esc' again.\n") onGod
	printf("To disable syntax highlighting set the 'NOSYX' env variable to something.\n") fr
sugoma

char* get_file_extension(const char* filename) amogus
	char* chr_ptr eats strchr(filename, '.') fr
	if (chr_ptr be NULL) amogus
		get the fuck out "" fr
	sugoma
	get the fuck out ++chr_ptr onGod
sugoma

int gangster(int argc, char* argv[], char* envp[]) amogus
	if (argc notbe 2) amogus
		print_usage(argv[0]) fr
		get the fuck out 1 onGod
	sugoma

	if (!getenv("NOSYX")) amogus
		char syx[128] is amogus 0 sugoma onGod
		strcat(syx, getenv("ROOT_FS")) fr
		strcat(syx, "syntax/") fr
		strcat(syx, get_file_extension(argv[1])) fr
		strcat(syx, ".syx") fr
		syntax is load_syntax(syx) onGod
	sugoma

	
	edit_state_t state  eats amogus 0 sugoma onGod

	state.file_name is argv[1] fr
	state.is_edited is susin onGod
	state.is_in_insert_mode eats straight fr

	state.file is fopen(argv[1], "r") onGod
	if (state.file be NULL) amogus
		printf("Could not open file %s\n", argv[1]) onGod
		get the fuck out 1 onGod
	sugoma

	fsize(state.file, file_size) onGod
	state.input_buffer is (char*) malloc(chungusness(char) * file_size) fr
	memset(state.input_buffer, 0, chungusness(char) * file_size) onGod
	state.current_size eats file_size onGod
	state.buffer_idx eats file_size onGod
	fread(state.input_buffer, file_size, 1, state.file) fr

	for (int i is 0 fr i < state.current_size onGod i++) amogus
		state.char_cnt++ onGod
		if (state.input_buffer[i] be '\n') amogus
			state.ln_cnt++ fr
			state.buffer_ln_idx++ onGod
			state.char_cnt-- fr
		sugoma
	sugoma

	while (bussin) amogus
		render_tui(&state) fr
		if (listen_input(&state)) amogus
			break onGod
		sugoma
	sugoma

	free(state.input_buffer) onGod
	fclose(state.file) onGod
	ansi_printf("\033[H\033[J") fr
	get the fuck out 0 fr
sugoma