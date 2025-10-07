#include <window/strinput.h>
#include <window.h>

bool strinput(psf1_font_t* font, strinput_t* data) {
    draw_string_window(font, data->x, data->y, data->buffer, data->fgcolor, data->bgcolor);

    char c = 0;
    if ((c = window_async_getc())) {
        switch (c) {
            case '\b':
                if (data->idx != 0) {
                    data->buffer[--data->idx] = 0;
                    draw_char_window(font, data->x + 8 * data->idx, data->y, ' ', data->fgcolor, data->bgcolor);
                }
                break;
            case '\n':
            case '\r':
                return false;
            default:
                data->buffer[data->idx++] = c;
                break;
        }
    }

    return true;
}