#pragma once

#include <types.h>

typedef struct {
    char text[128];
} test_state_t;

void test_init(window_instance_t* w);
void test_update(window_instance_t* w, event_t* event);
void test_draw(window_instance_t* w);
void test_cleanup(window_instance_t* w);
void register_test_window(void);

extern window_definition_t test_definition;
