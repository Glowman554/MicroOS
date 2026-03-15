#pragma once

#include <driver/char_output_driver.h>

extern char_output_driver_t full_screen_terminal_driver;

typedef struct full_screen_terminal_vterm {
    int x;
    int y;
    uint32_t color;
    uint32_t bgcolor;
    void* buffer;
} full_screen_terminal_vterm_t;

void full_screen_terminal_clear(char_output_driver_t* driver, int term);
void full_screen_terminal_erase_display(char_output_driver_t* driver, int term, enum ansi_erase_mode mode);
void full_screen_terminal_erase_line(char_output_driver_t* driver, int term, enum ansi_erase_mode mode);