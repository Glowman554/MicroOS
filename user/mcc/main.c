#include "chibi.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

// Returns the contents of a given file.
static char *read_file(char *path) {
	// Open and read the file.
	FILE *fp = fopen(path, "r");
	if (!fp) error("cannot open %s", path);

	// int filemax = 10 * 1024 * 1024;
	// char *buf = malloc(filemax);
	// int size = fread(buf, 1, filemax - 2, fp);
	// if (!feof(fp)) error("%s: file too large");

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

	char* buf = malloc(size + 2);
	fread(buf, 1, size, fp);

	// Make sure that the string ends with "\n\0".
	if (size == 0 || buf[size - 1] != '\n') buf[size++] = '\n';
	buf[size] = '\0';
	return buf;
}

char* get_tmp_path() {
	char* tmp = getenv("MCC_TMP");
	if (!tmp) {
		tmp = "tmp:";
	}

	return tmp;
}

int main(int argc, char **argv) {
	printf("MicroOS c compiler (based on chibicc)\n");

    char* output = "a.out";
    bool raw = true;

    int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-o") == 0) {
			if (idx + 1 < argc) {
				output = argv[idx + 1];
				idx++;
			} else {
				printf("Error: -o requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-r") == 0) {
            raw = false;
		} else {
			if (filename == NULL) {
				filename = argv[idx];
			} else {
				printf("Error: Too many arguments\n");
				abort();
			}
		}

		idx++;
	}

    if (filename == NULL) {
        printf("No input specified!\n");
        return -1;
    }

	user_input = read_file(filename);
	token = tokenize();
	Program *prog = program();

	// Assign offsets to local variables.
	for (Function *fn = prog->fns; fn; fn = fn->next) {
		int offset = fn->has_varargs ? 56 : 0;
		for (VarList *vl = fn->locals; vl; vl = vl->next) {
			Var *var = vl->var;
			offset = align_to(offset, var->ty->align);
			offset += var->ty->size;
			var->offset = offset;
		}
		fn->stack_size = align_to(offset, 8);
	}
 
	char out[128] = { 0 };
	sprintf(out, "%s/%d.asm", get_tmp_path(), time(NULL));
	FILE* outf = fopen(out, "w");

	// Traverse the AST to emit assembly.
	codegen(prog, outf, raw);

	fclose(outf);

	char fasm_cmd[128] = { 0 };
	sprintf(fasm_cmd, "fasm %s %s", out, output);

	system(fasm_cmd);

	return 0;
}