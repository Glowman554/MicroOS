#include <ps2_keyboard.h>
#include <ps2_mouse.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
    register_driver((driver_t*) get_ps2_driver());
    register_driver((driver_t*) get_ps2_mouse_driver());
}

define_module("ps2", main, stage_driver, NULL);
