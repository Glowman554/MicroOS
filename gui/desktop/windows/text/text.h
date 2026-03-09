#ifndef TEXT_H
#define TEXT_H

#include <types.h>

typedef struct {
    char text[128];
} text_state_t;

void text_init(window_instance_t* w);
void text_update(window_instance_t* w, event_t* event);
void text_draw(window_instance_t* w);
void text_cleanup(window_instance_t* w);
void register_text_window(void);

extern window_definition_t text_definition;

#endif // TEXT_H
