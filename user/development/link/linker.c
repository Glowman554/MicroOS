#include <linker.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t* read_file(const char* path, uint32_t* out_size) {
	FILE* f = fopen(path, "rb");
	if (!f) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	uint32_t sz = (uint32_t) ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* buf = (uint8_t*) malloc(sz);
	if (!buf) {
		fclose(f);
		return NULL;
	}

	if (fread(buf, 1, sz, f) != sz) {
		free(buf);
		fclose(f);
		return NULL;
	}
	fclose(f);

	*out_size = sz;
	return buf;
}

void linker_init(linker_state_t* state) {
	memset(state, 0, sizeof(linker_state_t));
	state->load_addr = DEFAULT_LOAD_ADDR;
	state->entry_name = "_start";
	state->output_file = "a.out";
}

int linker_find_symbol(linker_state_t* state, const char* name) {
	for (int i = 0; i < state->num_symbols; i++) {
		if (strcmp(state->symbols[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}

int linker_add_symbol(linker_state_t* state, const char* name, uint32_t value, int defined, int obj_idx, int sec_idx, uint8_t bind, uint8_t type) {
	int idx = linker_find_symbol(state, name);

	if (idx >= 0) {
		global_symbol_t* existing = &state->symbols[idx];
		if (defined) {
			if (existing->defined && bind == STB_GLOBAL && existing->bind == STB_GLOBAL) {
				fprintf(stderr, "linker: multiple definition of '%s'\n", name);
				return -1;
			}

			if (!existing->defined || existing->bind == STB_WEAK) {
				existing->value = value;
				existing->defined = 1;
				existing->obj_idx = obj_idx;
				existing->sec_idx = sec_idx;
				existing->bind = bind;
				existing->type = type;
			}
		}
		return idx;
	}

	if (state->num_symbols >= MAX_SYMBOLS) {
		fprintf(stderr, "linker: too many symbols\n");
		return -1;
	}

	idx = state->num_symbols++;
	global_symbol_t* sym = &state->symbols[idx];
	memset(sym, 0, sizeof(global_symbol_t));

	size_t nlen = strlen(name);
	if (nlen >= sizeof(sym->name)) {
		nlen = sizeof(sym->name) - 1;
	}
	memcpy(sym->name, name, nlen);
	sym->name[nlen] = '\0';

	sym->value = value;
	sym->defined = defined;
	sym->obj_idx = obj_idx;
	sym->sec_idx = sec_idx;
	sym->bind = bind;
	sym->type = type;
	return idx;
}

const char* get_section_name(uint8_t* data, elf32_ehdr_t* ehdr, elf32_shdr_t* shdr) {
	if (ehdr->sh_str_table_index == 0) {
		return "";
	}

	elf32_shdr_t* shstrtab = (elf32_shdr_t*)(data + ehdr->sh_offset + ehdr->sh_str_table_index * ehdr->sh_entry_size);
	if (shdr->name >= shstrtab->size) {
		return "";
	}

	return (const char*)(data + shstrtab->offset + shdr->name);
}

int classify_section(const char* name, uint32_t flags, uint32_t type) {
	if (type == SHT_NOBITS) {
		return 3; // bss
	}

	if (flags & SHF_EXEC) {
		return 0; // text
	}
	
	if (strcmp(name, ".rodata") == 0 || strncmp(name, ".rodata.", 8) == 0) {
		return 1; // rodata
	}
	
	if (flags & SHF_WRITE) {
		return 2; // data
	}
	
	if (strcmp(name, ".text") == 0 || strncmp(name, ".text.", 6) == 0) {
		return 0;
	}
	
	if (strcmp(name, ".data") == 0 || strncmp(name, ".data.", 6) == 0) {
		return 2;
	}
	
	if (strcmp(name, ".bss") == 0 || strncmp(name, ".bss.", 5) == 0) {
		return 3;
	}
	
	if (flags & SHF_ALLOC) {
		return 1;
	}

	return -1;
}

int parse_elf_object(linker_state_t* state, int obj_idx) {
	input_object_t* obj = &state->objects[obj_idx];
	uint8_t* data = obj->data;
	uint32_t fsize = obj->size;

	if (fsize < sizeof(elf32_ehdr_t)) {
		fprintf(stderr, "linker: %s: file too small\n", obj->filename);
		return -1;
	}

	elf32_ehdr_t* ehdr = (elf32_ehdr_t*) data;
	if (ehdr->magic != ELF_MAGIC) {
		fprintf(stderr, "linker: %s: not an ELF file\n", obj->filename);
		return -1;
	}

	if (ehdr->type != ET_REL) {
		fprintf(stderr, "linker: %s: not a relocatable object (type=%d)\n", obj->filename, ehdr->type);
		return -1;
	}

	obj->section_start = state->num_input_sections;
	obj->section_count = 0;

	int* sec_map = (int*) malloc(ehdr->sh_entry_count * sizeof(int));
	if (!sec_map) {
		fprintf(stderr, "linker: out of memory\n");
		return -1;
	}

	for (int i = 0; i < ehdr->sh_entry_count; i++) {
		sec_map[i] = -1;
	}

	for (int i = 0; i < ehdr->sh_entry_count; i++) {
		elf32_shdr_t* shdr = (elf32_shdr_t*)(data + ehdr->sh_offset + i * ehdr->sh_entry_size);

		if (!(shdr->flags & SHF_ALLOC)) {
			continue;
		}

		if (shdr->type != SHT_PROGBITS && shdr->type != SHT_NOBITS) {
			continue;
		}

		const char* sname = get_section_name(data, ehdr, shdr);
		int group = classify_section(sname, shdr->flags, shdr->type);
		if (group < 0) {
			continue;
		}

		if (state->num_input_sections >= MAX_SECTIONS) {
			fprintf(stderr, "linker: too many sections\n");
			free(sec_map);
			return -1;
		}

		int isec = state->num_input_sections++;
		input_section_t* sec = &state->input_sections[isec];
		memset(sec, 0, sizeof(input_section_t));

		sec->obj_idx = obj_idx;
		sec->sec_idx = i;

		size_t nlen = strlen(sname);
		if (nlen >= sizeof(sec->name)) {
			nlen = sizeof(sec->name) - 1;
		}
		memcpy(sec->name, sname, nlen);
		sec->name[nlen] = '\0';

		sec->type = shdr->type;
		sec->flags = shdr->flags;
		sec->size = shdr->size;
		sec->addralign = shdr->addralign;
		if (sec->addralign == 0) {
			sec->addralign = 1;
		}
		sec->out_group = group;

		if (shdr->type == SHT_PROGBITS && shdr->size > 0) {
			sec->data = (uint8_t*) malloc(shdr->size);
			if (!sec->data) {
				fprintf(stderr, "linker: out of memory\n");
				free(sec_map);
				return -1;
			}
			memcpy(sec->data, data + shdr->offset, shdr->size);
		}

		sec_map[i] = isec;
		obj->section_count++;
	}

	for (int i = 0; i < ehdr->sh_entry_count; i++) {
		elf32_shdr_t* shdr = (elf32_shdr_t*)(data + ehdr->sh_offset + i * ehdr->sh_entry_size);

		if (shdr->type != SHT_SYMTAB) {
			continue;
		}

		elf32_shdr_t* strtab_hdr = (elf32_shdr_t*)(data + ehdr->sh_offset + shdr->link * ehdr->sh_entry_size);
		char* strtab = (char*)(data + strtab_hdr->offset);

		int nsyms = shdr->size / sizeof(elf32_sym_t);
		elf32_sym_t* syms = (elf32_sym_t*)(data + shdr->offset);

		obj->sym_map = (int*) malloc(nsyms * sizeof(int));
		if (!obj->sym_map) {
			fprintf(stderr, "linker: out of memory\n");
			free(sec_map);
			return -1;
		}
		obj->sym_count = nsyms;

		for (int j = 0; j < nsyms; j++) {
			obj->sym_map[j] = -1;

			elf32_sym_t* sym = &syms[j];
			uint8_t bind = ELF_ST_BIND(sym->info);
			uint8_t stype = ELF_ST_TYPE(sym->info);

			if (j == 0) {
				continue;
			}

			if (stype == STT_FILE) {
				continue;
			}

			const char* symname = strtab + sym->name;

			char secname_buf[128];
			if (stype == STT_SECTION) {
				if (sym->shndx < ehdr->sh_entry_count && sec_map[sym->shndx] >= 0) {
					const char* sn = state->input_sections[sec_map[sym->shndx]].name;

					sprintf(secname_buf, "__sec_%d_%d_%s", obj_idx, sym->shndx, sn);
					symname = secname_buf;
				} else {
					continue;
				}
			}

			if (symname[0] == '\0') {
				continue;
			}

			if (bind == STB_LOCAL) {
				if (sym->shndx == SHN_UNDEF) {
					continue;
				}

				char local_buf[192];
				sprintf(local_buf, "__local_%d_%d_%s", obj_idx, j, symname);

				int is_defined = (sym->shndx != SHN_UNDEF) ? 1 : 0;
				int mapped_sec = -1;
				if (sym->shndx != SHN_UNDEF && sym->shndx != SHN_ABS &&
				    sym->shndx < ehdr->sh_entry_count) {
					mapped_sec = sec_map[sym->shndx];
				}

				int gidx = linker_add_symbol(state, local_buf, sym->value, is_defined, obj_idx, mapped_sec, bind, stype);
				if (gidx < 0) {
					free(sec_map);
					return -1;
				}
				obj->sym_map[j] = gidx;
				continue;
			}

			int is_defined = (sym->shndx != SHN_UNDEF) ? 1 : 0;
			int mapped_sec = -1;
			if (sym->shndx != SHN_UNDEF && sym->shndx != SHN_ABS &&
			    sym->shndx < ehdr->sh_entry_count) {
				mapped_sec = sec_map[sym->shndx];
			}

			uint32_t sym_value = sym->value;
			if (sym->shndx == SHN_ABS) {
				mapped_sec = -1;
			}

			int gidx = linker_add_symbol(state, symname, sym_value, is_defined, obj_idx, mapped_sec, bind, stype);
			if (gidx < 0) {
				free(sec_map);
				return -1;
			}
			obj->sym_map[j] = gidx;
		}
	}

	for (int i = 0; i < ehdr->sh_entry_count; i++) {
		elf32_shdr_t* shdr = (elf32_shdr_t*)(data + ehdr->sh_offset + i * ehdr->sh_entry_size);

		if (shdr->type != SHT_REL && shdr->type != SHT_RELA) {
			continue;
		}

		int target_elf_sec = shdr->info;
		int target_isec = -1;
		if (target_elf_sec < ehdr->sh_entry_count) {
			target_isec = sec_map[target_elf_sec];
		}
		if (target_isec < 0) {
			continue; 
		}

		if (shdr->type == SHT_REL) {
			int nrels = shdr->size / sizeof(elf32_rel_t);
			elf32_rel_t* rels = (elf32_rel_t*)(data + shdr->offset);

			for (int j = 0; j < nrels; j++) {
				if (state->num_relocs >= MAX_RELOCS) {
					fprintf(stderr, "linker: too many relocations\n");
					free(sec_map);
					return -1;
				}

				int sym_local = ELF_R_SYM(rels[j].info);
				int gsym = -1;
				if (sym_local >= 0 && sym_local < obj->sym_count) {
					gsym = obj->sym_map[sym_local];
				}

				if (gsym < 0) {
					continue;
				}

				pending_reloc_t* r = &state->relocs[state->num_relocs++];
				r->isec_idx = target_isec;
				r->offset = rels[j].offset;
				r->type = ELF_R_TYPE(rels[j].info);
				r->sym_idx = gsym;
				r->addend = 0;
				r->has_addend = 0;
			}
		} else {
			int nrels = shdr->size / sizeof(elf32_rela_t);
			elf32_rela_t* rels = (elf32_rela_t*)(data + shdr->offset);

			for (int j = 0; j < nrels; j++) {
				if (state->num_relocs >= MAX_RELOCS) {
					fprintf(stderr, "linker: too many relocations\n");
					free(sec_map);
					return -1;
				}

				int sym_local = ELF_R_SYM(rels[j].info);
				int gsym = -1;
				if (sym_local >= 0 && sym_local < obj->sym_count) {
					gsym = obj->sym_map[sym_local];
				}

				if (gsym < 0) {
					continue;
				}

				pending_reloc_t* r = &state->relocs[state->num_relocs++];
				r->isec_idx = target_isec;
				r->offset = rels[j].offset;
				r->type = ELF_R_TYPE(rels[j].info);
				r->sym_idx = gsym;
				r->addend = rels[j].addend;
				r->has_addend = 1;
			}
		}
	}

	free(sec_map);
	return 0;
}

int linker_add_object(linker_state_t* state, const char* filename) {
	if (state->num_objects >= MAX_INPUT_FILES) {
		fprintf(stderr, "linker: too many input files\n");
		return -1;
	}

	uint32_t fsize = 0;
	uint8_t* data = read_file(filename, &fsize);
	if (!data) {
		fprintf(stderr, "linker: cannot open '%s'\n", filename);
		return -1;
	}

	printf("linker: adding object '%s' (size=%d bytes)\n", filename, fsize);

	int idx = state->num_objects++;
	input_object_t* obj = &state->objects[idx];
	memset(obj, 0, sizeof(input_object_t));

	size_t nlen = strlen(filename);
	if (nlen >= sizeof(obj->filename)) {
		nlen = sizeof(obj->filename) - 1;
	}
	memcpy(obj->filename, filename, nlen);
	obj->filename[nlen] = '\0';

	obj->data = data;
	obj->size = fsize;

	return parse_elf_object(state, idx);
}

int ar_parse_size(const char* field, int len) {
	int val = 0;
	for (int i = 0; i < len; i++) {
		if (field[i] >= '0' && field[i] <= '9') {
			val = val * 10 + (field[i] - '0');
		} else {
			break;
		}
	}
	return val;
}

int has_undefined_symbols(linker_state_t* state) {
	for (int i = 0; i < state->num_symbols; i++) {
		if (!state->symbols[i].defined && state->symbols[i].bind != STB_WEAK) {
			return 1;
		}
	}
	return 0;
}

int add_object_from_archive_member(linker_state_t* state, uint8_t* member_data, uint32_t member_size, const char* ar_name, const char* member_name) {
	if (state->num_objects >= MAX_INPUT_FILES) {
		fprintf(stderr, "linker: too many input files\n");
		return -1;
	}

	if (member_size < sizeof(elf32_ehdr_t)) {
		return 0;
	}

	elf32_ehdr_t* ehdr = (elf32_ehdr_t*) member_data;
	if (ehdr->magic != ELF_MAGIC || ehdr->type != ET_REL) {
		return 0;
	}

	uint8_t* data_copy = (uint8_t*) malloc(member_size);
	if (!data_copy) {
		fprintf(stderr, "linker: out of memory\n");
		return -1;
	}
	memcpy(data_copy, member_data, member_size);

	int idx = state->num_objects++;
	input_object_t* obj = &state->objects[idx];
	memset(obj, 0, sizeof(input_object_t));

	{
		size_t ar_len = strlen(ar_name);
		size_t mem_len = strlen(member_name);
		size_t max_len = sizeof(obj->filename) - 1;
		if (ar_len + mem_len + 2 > max_len) {
			if (ar_len > max_len - mem_len - 3) {
				ar_len = max_len - mem_len - 3;
			}
		}
		memcpy(obj->filename, ar_name, ar_len);
		obj->filename[ar_len] = '(';
		memcpy(obj->filename + ar_len + 1, member_name, mem_len);
		obj->filename[ar_len + 1 + mem_len] = ')';
		obj->filename[ar_len + 2 + mem_len] = '\0';
	}

	obj->data = data_copy;
	obj->size = member_size;

	return parse_elf_object(state, idx);
}

int member_defines_needed_symbol(linker_state_t* state, uint8_t* member_data, uint32_t member_size) {
	if (member_size < sizeof(elf32_ehdr_t)) {
		return 0;
	}

	elf32_ehdr_t* ehdr = (elf32_ehdr_t*) member_data;
	if (ehdr->magic != ELF_MAGIC || ehdr->type != ET_REL) {
		return 0;
	}

	for (int i = 0; i < ehdr->sh_entry_count; i++) {
		elf32_shdr_t* shdr = (elf32_shdr_t*)(member_data + ehdr->sh_offset + i * ehdr->sh_entry_size);

		if (shdr->type != SHT_SYMTAB) {
			continue;
		}

		elf32_shdr_t* strtab_hdr = (elf32_shdr_t*)(member_data + ehdr->sh_offset + shdr->link * ehdr->sh_entry_size);
		char* strtab = (char*)(member_data + strtab_hdr->offset);

		int nsyms = shdr->size / sizeof(elf32_sym_t);
		elf32_sym_t* syms = (elf32_sym_t*)(member_data + shdr->offset);

		for (int j = 1; j < nsyms; j++) {
			uint8_t bind = ELF_ST_BIND(syms[j].info);
			if (bind != STB_GLOBAL && bind != STB_WEAK) {
				continue;
			}
			if (syms[j].shndx == SHN_UNDEF) {
				continue;
			}

			const char* symname = strtab + syms[j].name;
			int gidx = linker_find_symbol(state, symname);
			if (gidx >= 0 && !state->symbols[gidx].defined) {
				return 1;
			}
		}
	}

	return 0;
}

int process_archive(linker_state_t* state, const char* path) {
	uint32_t fsize = 0;
	uint8_t* ar_data = read_file(path, &fsize);
	if (!ar_data) {
		return -1;
	}

	printf("linker: processing archive '%s' (size=%d bytes)\n", path, fsize);

	if (fsize < AR_MAGIC_LEN || memcmp(ar_data, AR_MAGIC, AR_MAGIC_LEN) != 0) {
		fprintf(stderr, "linker: %s: not a valid archive\n", path);
		free(ar_data);
		return -1;
	}

	int changed = 1;
	while (changed) {
		changed = 0;
		uint32_t pos = AR_MAGIC_LEN;

		while (pos + sizeof(ar_header_t) <= fsize) {
			if (pos & 1) {
				pos++;
			}
			if (pos + sizeof(ar_header_t) > fsize) {
				break;
			}

			ar_header_t* hdr = (ar_header_t*)(ar_data + pos);

			if (hdr->fmag[0] != '`' || hdr->fmag[1] != '\n') {
				break;
			}

			int member_size = ar_parse_size(hdr->size, 10);
			uint32_t member_offset = pos + sizeof(ar_header_t);

			char member_name[17];
			memcpy(member_name, hdr->name, 16);
			member_name[16] = '\0';

			{
				int k = 15;
				while (k >= 0 && (member_name[k] == ' ' || member_name[k] == '/')) {
					member_name[k] = '\0';
					k--;
				}
			}

			if (member_name[0] == '/' || member_name[0] == ' ' || strcmp(member_name, "") == 0) {
				pos = member_offset + member_size;
				continue;
			}

			if (member_offset + (uint32_t)member_size > fsize) {
				break;
			}

			if (member_defines_needed_symbol(state, ar_data + member_offset, member_size)) {
				int ret = add_object_from_archive_member(state,
					ar_data + member_offset, member_size, path, member_name);
				if (ret < 0) {
					free(ar_data);
					return -1;
				}
				changed = 1;
			}

			pos = member_offset + member_size;
		}
	}

	free(ar_data);
	return 0;
}

int linker_resolve_libraries(linker_state_t* state) {
	if (state->entry_name && state->entry_name[0] != '\0') {
		int entry_idx = linker_find_symbol(state, state->entry_name);
		if (entry_idx < 0) {
			if (linker_add_symbol(state, state->entry_name, 0, 0, -1, -1,
				STB_GLOBAL, STT_NOTYPE) < 0) {
				return -1;
			}
		}
	}

	if (!has_undefined_symbols(state)) {
		return 0;
	}

	for (int i = 0; i < state->num_libs; i++) {
		int found = 0;
		for (int j = 0; j < state->num_lib_paths; j++) {
			char path[512];
			sprintf(path, "%s/lib%s.a", state->lib_paths[j], state->libs[i]);

			uint32_t dummy;
			uint8_t* test = read_file(path, &dummy);
			if (test) {
				free(test);
				if (process_archive(state, path) < 0) {
					return -1;
				}
				found = 1;
				break;
			}
		}
		if (!found) {
			fprintf(stderr, "linker: cannot find -l%s\n", state->libs[i]);
			return -1;
		}
	}

	return 0;
}

uint32_t align_up(uint32_t val, uint32_t align) {
	if (align <= 1) {
		return val;
	}
	return (val + align - 1) & ~(align - 1);
}

int linker_layout(linker_state_t* state) {
	uint32_t current_addr = state->load_addr;

	for (int group = 0; group <= 3; group++) {
		for (int i = 0; i < state->num_input_sections; i++) {
			input_section_t* sec = &state->input_sections[i];
			if (sec->out_group != group) {
				continue;
			}
			current_addr = align_up(current_addr, sec->addralign);
			sec->out_vaddr = current_addr;
			sec->out_offset = 0; 
			current_addr += sec->size;
		}

		current_addr = align_up(current_addr, 16);
	}

	{
		for (int i = 0; i < state->num_symbols; i++) {
			global_symbol_t* sym = &state->symbols[i];
			if (!sym->defined) {
				continue;
			}

			if (sym->sec_idx >= 0 && sym->sec_idx < state->num_input_sections) {
				sym->value += state->input_sections[sym->sec_idx].out_vaddr;
			}
		}
	}

	return 0;
}

int linker_relocate(linker_state_t* state) {
	for (int i = 0; i < state->num_relocs; i++) {
		pending_reloc_t* r = &state->relocs[i];
		input_section_t* sec = &state->input_sections[r->isec_idx];
		global_symbol_t* sym = &state->symbols[r->sym_idx];

		if (!sym->defined) {
			fprintf(stderr, "linker: undefined reference to '%s'\n", sym->name);
			return -1;
		}

		if (!sec->data) {
			fprintf(stderr, "linker: relocation in NOBITS section '%s'\n", sec->name);
			return -1;
		}

		if (r->offset + 4 > sec->size) {
			fprintf(stderr, "linker: relocation offset out of bounds in '%s'\n", sec->name);
			return -1;
		}

		uint32_t S = sym->value;
		uint32_t P = sec->out_vaddr + r->offset;
		uint32_t* patch = (uint32_t*)(sec->data + r->offset);

		int32_t A;
		if (r->has_addend) {
			A = r->addend;
		} else {
			A = (int32_t)(*patch);
		}

		switch (r->type) {
			case R_386_32:
				*patch = S + A;
				break;
			case R_386_PC32:
			case R_386_PLT32:
				*patch = S + A - P;
				break;
			case R_386_NONE:
				break;
			default:
				fprintf(stderr, "linker: unsupported relocation type %d\n", r->type);
				return -1;
		}
	}

	return 0;
}

int linker_write_elf(linker_state_t* state, const char* output) {
	int entry_idx = linker_find_symbol(state, state->entry_name);
	uint32_t entry_addr = state->load_addr;
	if (entry_idx >= 0 && state->symbols[entry_idx].defined) {
		entry_addr = state->symbols[entry_idx].value;
	} else {
		fprintf(stderr, "linker: warning: entry symbol '%s' not found, using load address\n", state->entry_name);
	}

	uint32_t group_start[4] = {0};
	uint32_t group_end[4] = {0};
	int group_used[4] = {0};

	{
		for (int i = 0; i < state->num_input_sections; i++) {
			input_section_t* sec = &state->input_sections[i];
			int g = sec->out_group;
			if (g < 0 || g > 3) {
				continue;
			}
			uint32_t sec_end = sec->out_vaddr + sec->size;
			if (!group_used[g]) {
				group_start[g] = sec->out_vaddr;
				group_end[g] = sec_end;
				group_used[g] = 1;
			} else {
				if (sec->out_vaddr < group_start[g]) {
					group_start[g] = sec->out_vaddr;
				}
				if (sec_end > group_end[g]) {
					group_end[g] = sec_end;
				}
			}
		}
	}

	int num_phdrs = 0;
	elf32_phdr_t phdrs[2];
	memset(phdrs, 0, sizeof(phdrs));

	uint32_t text_start = state->load_addr;
	uint32_t text_end = text_start;
	if (group_used[0]) {
		text_start = group_start[0];
		text_end = group_end[0];
	}
	if (group_used[1]) {
		if (!group_used[0]) {
			text_start = group_start[1];
		}
		if (group_end[1] > text_end) {
			text_end = group_end[1];
		}
	}

	uint32_t data_start = text_end;
	uint32_t data_end = data_start;
	if (group_used[2]) {
		data_start = group_start[2];
		data_end = group_end[2];
	}
	if (group_used[3]) {
		if (!group_used[2]) {
			data_start = group_start[3];
		}
		if (group_end[3] > data_end) {
			data_end = group_end[3];
		}
	}

	if (text_end > text_start) {
		num_phdrs++;
	}
	if (data_end > data_start) {
		num_phdrs++;
	}
	if (num_phdrs == 0) {
		/* At least one segment */
		num_phdrs = 1;
	}

	uint32_t headers_size = sizeof(elf32_ehdr_t) + num_phdrs * sizeof(elf32_phdr_t);

	uint32_t first_section_vaddr = state->load_addr;
	{
		int found = 0;
		for (int i = 0; i < state->num_input_sections; i++) {
			if (!found || state->input_sections[i].out_vaddr < first_section_vaddr) {
				first_section_vaddr = state->input_sections[i].out_vaddr;
				found = 1;
			}
		}
	}

	uint32_t page_size = 0x1000;
	uint32_t vaddr_page_offset = first_section_vaddr & (page_size - 1);

	uint32_t first_section_file_offset;
	if (headers_size <= vaddr_page_offset) {
		first_section_file_offset = vaddr_page_offset;
	} else {
		first_section_file_offset = align_up(headers_size, page_size) + vaddr_page_offset;
	}

	{
		for (int i = 0; i < state->num_input_sections; i++) {
			input_section_t* sec = &state->input_sections[i];
			sec->out_offset = first_section_file_offset + (sec->out_vaddr - first_section_vaddr);
		}
	}

	int phdr_idx = 0;

	if (text_end > text_start) {
		uint32_t seg_file_start = first_section_file_offset + (text_start - first_section_vaddr);
		uint32_t seg_vaddr = text_start;

		uint32_t page_base_file = seg_file_start & ~(page_size - 1);
		uint32_t page_base_vaddr = seg_vaddr & ~(page_size - 1);

		phdrs[phdr_idx].type = PT_LOAD;
		phdrs[phdr_idx].offset = page_base_file;
		phdrs[phdr_idx].virt_addr = page_base_vaddr;
		phdrs[phdr_idx].phys_addr = page_base_vaddr;
		phdrs[phdr_idx].file_size = first_section_file_offset + (text_end - first_section_vaddr) - page_base_file;
		phdrs[phdr_idx].mem_size = phdrs[phdr_idx].file_size;
		phdrs[phdr_idx].flags = PF_R | PF_X;
		phdrs[phdr_idx].alignment = page_size;
		phdr_idx++;
	}

	if (data_end > data_start) {
		uint32_t seg_file_start = first_section_file_offset + (data_start - first_section_vaddr);
		uint32_t bss_file_size = 0;
		if (group_used[2]) {
			bss_file_size = group_end[2] - data_start;
		}

		phdrs[phdr_idx].type = PT_LOAD;
		phdrs[phdr_idx].offset = seg_file_start;
		phdrs[phdr_idx].virt_addr = data_start;
		phdrs[phdr_idx].phys_addr = data_start;
		phdrs[phdr_idx].file_size = bss_file_size;
		phdrs[phdr_idx].mem_size = data_end - data_start;
		phdrs[phdr_idx].flags = PF_R | PF_W;
		phdrs[phdr_idx].alignment = page_size;
		phdr_idx++;
	}

	if (phdr_idx == 0) {
		phdrs[0].type = PT_LOAD;
		phdrs[0].offset = 0;
		phdrs[0].virt_addr = state->load_addr;
		phdrs[0].phys_addr = state->load_addr;
		phdrs[0].file_size = 0;
		phdrs[0].mem_size = 0;
		phdrs[0].flags = PF_R | PF_X;
		phdrs[0].alignment = page_size;
		phdr_idx = 1;
	}

	uint32_t total_file_size = headers_size;
	{
		for (int i = 0; i < state->num_input_sections; i++) {
			input_section_t* sec = &state->input_sections[i];
			if (sec->type == SHT_NOBITS) {
				continue;
			}
			uint32_t sec_end = sec->out_offset + sec->size;
			if (sec_end > total_file_size) {
				total_file_size = sec_end;
			}
		}
	}

	uint8_t* out_buf = (uint8_t*) calloc(1, total_file_size);
	if (!out_buf) {
		fprintf(stderr, "linker: out of memory for output\n");
		return -1;
	}

	elf32_ehdr_t* ehdr = (elf32_ehdr_t*) out_buf;
	ehdr->magic = ELF_MAGIC;
	ehdr->e_class = 1;      /* ELFCLASS32 */
	ehdr->data = 1;         /* ELFDATA2LSB */
	ehdr->hdr_version = 1;  /* EV_CURRENT */
	ehdr->os_abi = 0;       /* ELFOSABI_NONE */
	memset(ehdr->padding, 0, 8);
	ehdr->type = ET_EXEC;
	ehdr->machine = EM_386;
	ehdr->version = 1;
	ehdr->entry = entry_addr;
	ehdr->ph_offset = sizeof(elf32_ehdr_t);
	ehdr->sh_offset = 0;    /* no section headers */
	ehdr->flags = 0;
	ehdr->header_size = sizeof(elf32_ehdr_t);
	ehdr->ph_entry_size = sizeof(elf32_phdr_t);
	ehdr->ph_entry_count = phdr_idx;
	ehdr->sh_entry_size = sizeof(elf32_shdr_t);
	ehdr->sh_entry_count = 0;
	ehdr->sh_str_table_index = 0;

	memcpy(out_buf + sizeof(elf32_ehdr_t), phdrs, phdr_idx * sizeof(elf32_phdr_t));

	{
		for (int i = 0; i < state->num_input_sections; i++) {
			input_section_t* sec = &state->input_sections[i];
			if (sec->type == SHT_NOBITS || !sec->data) {
				continue;
			}
			if (sec->out_offset + sec->size <= total_file_size) {
				memcpy(out_buf + sec->out_offset, sec->data, sec->size);
			}
		}
	}

	FILE* f = fopen(output, "wb");
	if (!f) {
		fprintf(stderr, "linker: cannot create '%s'\n", output);
		free(out_buf);
		return -1;
	}
	fwrite(out_buf, 1, total_file_size, f);
	fclose(f);

	free(out_buf);

	printf("linker: wrote %s (%u bytes, entry=0x%x)\n", output, total_file_size, entry_addr);
	return 0;
}
