#include <native.h>
#include <window.h>
#include <window/font.h>
#include <window/fpic.h>
#include <window/vconsole.h>

#include <stdlib.h>

void native_set_pixel_window(struct vm_instance* vm) {
    uint32_t color = (uint32_t) stack_pop(vm);
    uint32_t y = (uint32_t) stack_pop(vm);
    uint32_t x = (uint32_t) stack_pop(vm);
    set_pixel_window(x, y, color);
    stack_push(vm, 0);
}

void native_get_pixel_window(struct vm_instance* vm) {
    uint32_t y = (uint32_t) stack_pop(vm);
    uint32_t x = (uint32_t) stack_pop(vm);
    stack_push(vm, get_pixel_window(x, y));
}

void native_draw_char_window(struct vm_instance* vm) {
    uint32_t bgcolor = (uint32_t) stack_pop(vm);
    uint32_t color = (uint32_t) stack_pop(vm);
    char c = (char) stack_pop(vm);
    uint32_t y = (uint32_t) stack_pop(vm);
    uint32_t x = (uint32_t) stack_pop(vm);
    psf1_font_t* font = (psf1_font_t*) (uint32_t) stack_pop(vm);
    draw_char_window(font, x, y, c, color, bgcolor);
    stack_push(vm, 0);
}

void native_draw_string_window(struct vm_instance* vm) {
    uint32_t bgcolor = (uint32_t) stack_pop(vm);
    uint32_t color = (uint32_t) stack_pop(vm);
    const char* s = (const char*) (uint32_t) stack_pop(vm);
    uint32_t y = (uint32_t) stack_pop(vm);
    uint32_t x = (uint32_t) stack_pop(vm);
    psf1_font_t* font = (psf1_font_t*) (uint32_t) stack_pop(vm);
    draw_string_window(font, x, y, s, color, bgcolor);
    stack_push(vm, 0);
}

void native_draw_line_window(struct vm_instance* vm) {
    uint32_t color = (uint32_t) stack_pop(vm);
    uint32_t y2 = (uint32_t) stack_pop(vm);
    uint32_t x2 = (uint32_t) stack_pop(vm);
    uint32_t y1 = (uint32_t) stack_pop(vm);
    uint32_t x1 = (uint32_t) stack_pop(vm);
    draw_line_window(x1, y1, x2, y2, color);
    stack_push(vm, 0);
}

void native_window_init(struct vm_instance* vm) {
    const char* name = (const char*) (uint32_t) stack_pop(vm);
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    int width = (int) stack_pop(vm);
    int height = (int) stack_pop(vm);
    window_init(height, width, x, y, name);
    stack_push(vm, 0);
}

void native_window_async_getc(struct vm_instance* vm) {
    stack_push(vm, window_async_getc());
}
void native_window_async_getarrw(struct vm_instance* vm) {
    stack_push(vm, window_async_getarrw());
}

void native_window_mouse_info(struct vm_instance* vm) {
    int64_t* out = (int64_t*) (uint32_t) stack_pop(vm);
    
    mouse_info_t info;
    window_mouse_info(&info);

    out[0] = info.x;
    out[1] = info.y;
    out[2] = info.button_left;
    out[3] = info.button_right;
    out[4] = info.button_middle;

    stack_push(vm, 0);
}

void native_window_optimize(struct vm_instance* vm) {
    window_optimize();
    stack_push(vm, 0);
}

void native_window_redrawn(struct vm_instance* vm) {
    stack_push(vm, window_redrawn());
}

void native_window_open_prog_request(struct vm_instance* vm) {
    const char* file = (const char*) (uint32_t) stack_pop(vm);
    window_open_prog_request(file);
    stack_push(vm, 0);
}

void native_window_clear(struct vm_instance* vm) {
    uint32_t color = (uint32_t) stack_pop(vm);
    window_clear(color);
    stack_push(vm, 0);
}

void native_load_psf1_font(struct vm_instance* vm) {
    const char* path = (const char*) (uint32_t) stack_pop(vm);

    psf1_font_t* fontPtr = malloc(sizeof(psf1_font_t));
    *fontPtr = load_psf1_font(path);

    stack_push(vm, (int32_t) fontPtr);
}

void native_load_fpic_window(struct vm_instance* vm) {
    char* file = (char*) (uint32_t) stack_pop(vm);
    stack_push(vm, (int32_t) load_fpic_window(file));
}

void native_get_pixel_fpic_window(struct vm_instance* vm) {
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    fpic_image_t* pic = (fpic_image_t*) (uint32_t) stack_pop(vm);
    stack_push(vm, get_pixel_fpic_window(pic, x, y));
}

void native_draw_fpic_window(struct vm_instance* vm) {
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    fpic_image_t* pic = (fpic_image_t*) (uint32_t) stack_pop(vm);
    draw_fpic_window(pic, x, y);
    stack_push(vm, 0);
}

void native_draw_fpic_scaled_window(struct vm_instance* vm) {
    int scale = (int) stack_pop(vm);
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    fpic_image_t* pic = (fpic_image_t*) (uint32_t) stack_pop(vm);
    draw_fpic_scaled_window(pic, x, y, scale);
    stack_push(vm, 0);
}

void native_vconsole_init(struct vm_instance* vm) {
    psf1_font_t* font = (psf1_font_t*) (uint32_t) stack_pop(vm);
    vconsole_init(font);
    stack_push(vm, 0);
}

void native_vconsole_clear(struct vm_instance* vm) {
    vconsole_clear();
    stack_push(vm, 0);
}

void native_vconsole_ansi_process(struct vm_instance* vm) {
    char c = (char) stack_pop(vm);
    stack_push(vm, vconsole_ansi_process(c));
}

void native_vconsole_putc(struct vm_instance* vm) {
    char c = (char) stack_pop(vm);
    vconsole_putc(c);
    stack_push(vm, 0);
}

void native_vconsole_puts(struct vm_instance* vm) {
    const char* s = (const char*) (uint32_t) stack_pop(vm);
    vconsole_puts(s);
    stack_push(vm, 0);
}

void native_vconsole_set_color(struct vm_instance* vm) {
    bool background = (bool) stack_pop(vm);
    uint32_t color = (uint32_t) stack_pop(vm);
    vconsole_set_color(color, background);
    stack_push(vm, 0);
}


void native_microos() {
    vm_native_register(789023, native_set_pixel_window);
    vm_native_register(789024, native_get_pixel_window);
    vm_native_register(789025, native_draw_char_window);
    vm_native_register(789026, native_draw_string_window);
    vm_native_register(789027, native_draw_line_window);
    vm_native_register(789028, native_window_init);
    vm_native_register(789029, native_window_async_getc);
    vm_native_register(789030, native_window_async_getarrw);
    vm_native_register(789031, native_window_mouse_info);
    vm_native_register(789032, native_window_optimize);
    vm_native_register(789033, native_window_redrawn);
    vm_native_register(789034, native_window_open_prog_request);
    vm_native_register(789035, native_window_clear);

    vm_native_register(789036, native_load_psf1_font);

    vm_native_register(789037, native_load_fpic_window);
    vm_native_register(789038, native_get_pixel_fpic_window);
    vm_native_register(789039, native_draw_fpic_window);
    vm_native_register(789040, native_draw_fpic_scaled_window);

    vm_native_register(789041, native_vconsole_init);
    vm_native_register(789042, native_vconsole_clear);
    vm_native_register(789043, native_vconsole_ansi_process);
    vm_native_register(789044, native_vconsole_putc);
    vm_native_register(789045, native_vconsole_puts);
    vm_native_register(789046, native_vconsole_set_color);
}