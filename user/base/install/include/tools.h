#pragma once

void copy_dir_across_fs(char* src_fs, char* dest_fs, char* path);
void copy_file_across_fs(char* src_fs, char* dest_fs, char* path, char* file);
void create_directory(char* dest_fs, char* path);
void write_text_file(char* dest_fs, char* path, char* text);