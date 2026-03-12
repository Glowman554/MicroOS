#pragma once

#include <types.h>
#include <stdbool.h>
#include <stdio.h>

#include <syntax.h>

typedef struct {
    char file_name[128];
    char* input_buffer;
    bool is_edited;
    bool is_in_insert_mode;

    unsigned int ln_cnt;
    unsigned int char_cnt;

    unsigned int buffer_ln_idx;
    unsigned int buffer_idx;

    unsigned int current_size;

    int scroll_offset;

    FILE* file;

    uint8_t* color;

    syntax_header_t* syntax;
} edit_state_t;

void edit_init(window_instance_t* w);
void edit_update(window_instance_t* w, event_t* event);
void edit_draw(window_instance_t* w);
void edit_cleanup(window_instance_t* w);
void edit_open_picker(void);
void edit_open(const char* path);

extern window_definition_t edit_definition;
