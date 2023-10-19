#include "../user/edit/include/syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_string(int* len, char** buf, const char* str) {
	int ret = *len;
	printf("Including string '%s' at %d in file\n", str, ret);

	int strl = strlen(str);

	*buf = realloc(*buf, ret + strl + 1);
	memcpy(&(*buf)[ret], str, strl + 1);

	*len = ret + strl + 1;

	return ret;
}

void save_syx(syntax_header_t* header, syntax_word_t* words, int len, char* buf, char* file) {
    printf("Saving in %s...\n", file);
	FILE* s = fopen(file, "wb");
	if (!s) {
		abort();
	}

    fwrite(header, sizeof(syntax_header_t), 1, s);
	fwrite(words, sizeof(syntax_word_t), header->num_words, s);
	fwrite(buf, len, 1, s);

	free(buf);
	fclose(s);

	printf("Final string table length is %d\n", len);
}

#define NEW_WORD(str, c) { .word_offset = write_string(&len, &buf, str), .color = c }

void write_c_syntax() {
	char* buf = NULL;
	int len = 0;

    syntax_word_t words[] = {
		NEW_WORD("auto", blue),
		NEW_WORD("char", blue),
		NEW_WORD("double", blue),
		NEW_WORD("float", blue),
		NEW_WORD("int", blue),
		NEW_WORD("short", blue),
		NEW_WORD("long", blue),
		NEW_WORD("void", blue),

		NEW_WORD("bool", blue),
		NEW_WORD("uint8_t", blue),
		NEW_WORD("uint16_t", blue),
		NEW_WORD("uint32_t", blue),
		NEW_WORD("uint64_t", blue),
		NEW_WORD("int8_t", blue),
		NEW_WORD("int16_t", blue),
		NEW_WORD("int32_t", blue),
		NEW_WORD("int64_t", blue),


		NEW_WORD("const", cyan),
		NEW_WORD("extern", cyan),
		NEW_WORD("register", cyan),
		NEW_WORD("signed", cyan),
		NEW_WORD("unsigned", cyan),
		NEW_WORD("volatile", cyan),
		NEW_WORD("static", cyan),

		NEW_WORD("break", magenta),
		NEW_WORD("case", magenta),
		NEW_WORD("continue", magenta),
		NEW_WORD("default", magenta),
		NEW_WORD("do", magenta),
		NEW_WORD("else", magenta),
		NEW_WORD("enum", magenta),
		NEW_WORD("for", magenta),
		NEW_WORD("goto", magenta),
		NEW_WORD("if", magenta),
		NEW_WORD("return", magenta),
		NEW_WORD("struct", magenta),
		NEW_WORD("switch", magenta),
		NEW_WORD("typedef", magenta),
		NEW_WORD("union", magenta),
		NEW_WORD("while", magenta),

		NEW_WORD("true", red),
		NEW_WORD("false", red),
	};

	syntax_header_t header = {
		.magic = 0xff << 24 | 'S' << 0 | 'Y' << 8 | 'X' << 16, // TODO,
		.single_line_comment = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, "//"),
			.sect_end_offset = write_string(&len, &buf, "\n"),
			.color = green
		},
		.multi_line_comment = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, "/*"),
			.sect_end_offset = write_string(&len, &buf, "*/"),
			.color = green
		},
		.string = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, "\""),
			.sect_end_offset = write_string(&len, &buf, "\""),
			.color = yellow,
			.skip_next = '\\'
		},
		.single_char = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, "'"),
			.sect_end_offset = write_string(&len, &buf, "'"),
			.color = yellow,
			.skip_next = '\\'
		},
		.brackets_start = write_string(&len, &buf, "({["),
		.brackets_end = write_string(&len, &buf, "]})"),
		.match_brackets = true,
		.num_words = sizeof(words) / sizeof(words[0])
	};

    save_syx(&header, words, len, buf, "c.syx");
}

void write_asm_syntax() {
	char* buf = NULL;
	int len = 0;

    syntax_word_t words[] = {
		NEW_WORD("ah", red),
		NEW_WORD("al", red),
		NEW_WORD("ch", red),
		NEW_WORD("cl", red),
		NEW_WORD("dh", red),
		NEW_WORD("dl", red),
		NEW_WORD("bh", red),
		NEW_WORD("bl", red),
		NEW_WORD("spl", red),
		NEW_WORD("bpl", red),
		NEW_WORD("sil", red),
		NEW_WORD("dil", red),

		NEW_WORD("ax", red),
		NEW_WORD("cx", red),
		NEW_WORD("dx", red),
		NEW_WORD("bx", red),
		NEW_WORD("sp", red),
		NEW_WORD("bp", red),
		NEW_WORD("si", red),
		NEW_WORD("di", red),

		NEW_WORD("eax", red),
		NEW_WORD("ecx", red),
		NEW_WORD("edx", red),
		NEW_WORD("ebx", red),
		NEW_WORD("esp", red),
		NEW_WORD("ebp", red),
		NEW_WORD("esi", red),
		NEW_WORD("edi", red),


		NEW_WORD("db", yellow),
		NEW_WORD("dw", yellow),
		NEW_WORD("dd", yellow),
		NEW_WORD("dq", yellow),

		NEW_WORD("mov", blue),
		NEW_WORD("add", blue),
		NEW_WORD("sub", blue),
		NEW_WORD("mul", blue),
		NEW_WORD("div", blue),
		NEW_WORD("inc", blue),
		NEW_WORD("dec", blue),
		NEW_WORD("imul", blue),
		NEW_WORD("idiv", blue),
		NEW_WORD("call", blue),
		NEW_WORD("ret", blue),
		NEW_WORD("push", blue),
		NEW_WORD("pop", blue),
		NEW_WORD("and", blue),
		NEW_WORD("or", blue),
		NEW_WORD("xor", blue),
		NEW_WORD("shl", blue),
		NEW_WORD("shr", blue),
		NEW_WORD("cmp", blue),
		NEW_WORD("je", blue),
		NEW_WORD("jne", blue),
		NEW_WORD("jg", blue),
		NEW_WORD("jge", blue),
		NEW_WORD("jl", blue),
		NEW_WORD("jle", blue),
		NEW_WORD("test", blue),
		NEW_WORD("jz", blue),
		NEW_WORD("jnz", blue),
		NEW_WORD("jmp", blue),
		NEW_WORD("ljmp", blue),
		NEW_WORD("nop", blue),
		NEW_WORD("int", blue),

		NEW_WORD("segment", cyan),
		NEW_WORD("entry", cyan),
		NEW_WORD("format", cyan),

	};

	syntax_header_t header = {
		.magic = 0xff << 24 | 'S' << 0 | 'Y' << 8 | 'X' << 16, // TODO,
		.single_line_comment = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, ";"),
			.sect_end_offset = write_string(&len, &buf, "\n"),
			.color = green
		},
		.multi_line_comment = {
			.active = false,
		},
		.string = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, "\""),
			.sect_end_offset = write_string(&len, &buf, "\""),
			.color = yellow,
			.skip_next = '\\'
		},
		.single_char = {
			.active = true,
			.sect_start_offset = write_string(&len, &buf, "'"),
			.sect_end_offset = write_string(&len, &buf, "'"),
			.color = yellow,
			.skip_next = '\\'
		},
		.brackets_start = write_string(&len, &buf, "["),
		.brackets_end = write_string(&len, &buf, "]"),
		.match_brackets = true,
		.num_words = sizeof(words) / sizeof(words[0])
	};

    save_syx(&header, words, len, buf, "asm.syx");
}

int main(int argc, char** argv) {
	write_c_syntax();
    write_asm_syntax();
}