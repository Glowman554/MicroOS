import sys
import struct
import json
import gzip


src_compress = """
#include <stdio.h>
#include <stdlib.h>
#include <tinf.h>
#include <stdbool.h>
#include <sys/file.h>

{{data}}

struct file_mapping {
	char* path;

	const unsigned char* content;
	int size;
};

{{mappings}}

char* command = {{command}};

char* license = {{license}};

char* directories[] = { {{directories}} };

static unsigned int read_le32(const unsigned char *p) {
	return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24);
}

int main() {
	bool auto_accept = getenv("ACCEPT_ALL") != NULL;
	printf("going to install {{name}} (%d files)\\n", sizeof(files) / sizeof(struct file_mapping));
	if (!auto_accept) {
		printf("do you want to continue? (y/n) ");
		char c = getchar();
		if (c != 'y') {
			printf("\\naborting\\n");
			return 0;
		}
	}

	printf("\\n");

	if (license != NULL && !auto_accept) {
		fputs(license, stdout);
		printf("do you accept the license? (y/n) ");
		char c = getchar();
		if (c != 'y') {
			printf("\\naborting\\n");
			return 0;
		}
	}

	printf("\\n");

	{
		char final_path[0xff] = { 0 };
		sprintf(final_path, "%sopt", getenv("ROOT_FS"));
		mkdir(final_path);
	}

	{
		char final_path[0xff] = { 0 };
		sprintf(final_path, "%sopt/bin", getenv("ROOT_FS"));
		mkdir(final_path);
	}

	for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
		char final_path[0xff] = { 0 };
		sprintf(final_path, "%sopt/%s", getenv("ROOT_FS"), directories[i]);

		printf("Creating directory %s...\\n", final_path);
		mkdir(final_path);
	}

	for (int i = 0; i < sizeof(files) / sizeof(struct file_mapping); i++) {
		struct file_mapping* file = &files[i];
		char final_path[0xff] = { 0 };
		sprintf(final_path, "%sopt/%s", getenv("ROOT_FS"), file->path);

		printf("Extracting %s... ", final_path);

		unsigned int dlen = read_le32(file->content + file->size - 4);
		char* dest = (char*) malloc(dlen);
		unsigned int outlen = dlen;
		int res = tinf_gzip_uncompress(dest, &outlen, file->content, file->size);
		
		if ((res != TINF_OK) || (outlen != dlen)) {
			printf("decompression failed\\n");
			free(dest);
			abort();
		}

		FILE* f = fopen(final_path, "wb");
		if (f == NULL) {
			printf("Failed to open file %s\\n", final_path);
			return 1;
		}

		fwrite(dest, 1, outlen, f);
		fclose(f);
		free(dest);

		printf("Done\\n");
	}

	if (command) {
		printf("running command: %s\\n", command);
		system(command);
	}

	return 0;
}
"""

def gen_data(file):
	# replace every caracter witch is not a-z, A-Z, 0-9, _, - with _
	name = file.split("/")[-1]
	normalized_name = "_"
	for c in name:
		if not (c.isalpha() or c.isdigit() or c == "_"):
			normalized_name += "_"
		else:
			normalized_name += c

	out = ""
	with open(file, 'rb') as f:
		data = f.read()
		data = gzip.compress(data)
		out += 'const unsigned char {}[] = {{\n'.format(normalized_name)
		for i in range(0, len(data), 16):
			out += '    '
			for j in range(i, i+16):
				if j < len(data):
					out += '0x{:02x}, '.format(data[j])
			out += '\n'
		out += '};\n'
		out += 'const unsigned int {}_len = {};\n'.format(normalized_name, len(data))
		out += '\n'
	return out, normalized_name

mapping = {}
installer_name = ""
command = "NULL"
license = "NULL"
with open(sys.argv[1], 'r') as f:
	json_data = json.load(f)
	mapping = json_data['mappings']
	installer_name = json_data['name']
	if 'command' in json_data:
		command = '"' + json_data['command'] + '"'
	if 'license' in json_data:
		with open(json_data['license'], 'r') as l:
			license = '"' + l.read().replace('\n', '\\n').replace('"', '\\"') + '"'

file_data = ""
file_names = {}
for file in mapping:
	print("Loading {}...".format(file))
	out, name = gen_data(file)
	file_data += out
	file_names[file] = name

mappings_struct = "struct file_mapping files[] = {\n"
for file in mapping:
	print("Mapping {}...".format(file))
	mappings_struct += '    {{ "{}", {}, {}_len }},\n'.format(mapping[file], file_names[file], file_names[file])
mappings_struct += "};"

directories = []
if "directories" in json_data:
	for i in json_data["directories"]:
		directories.append('"' + i + '"')

with open(sys.argv[2], 'w') as f:
	src = src_compress
	f.write(src.replace("{{data}}", file_data).replace("{{mappings}}", mappings_struct).replace("{{name}}", installer_name).replace("{{command}}", command).replace("{{license}}", license).replace("{{directories}}", ",".join(directories)))