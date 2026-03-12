#pragma once

#include <stdio.h>

void ftruncate(FILE* f);
bool resolve(char* path, char* output);
bool resolve_check(char* path, char* output, bool check_childs);

#define fsize(stream, size) fseek(stream, 0, SEEK_END); size_t size = ftell(stream); fseek(stream, 0, SEEK_SET);

void init_stdio();
void uninit_stdio();

void read_all_stdin(char** out, size_t* size);
void read_all_file(FILE* f, char** out, size_t* size);