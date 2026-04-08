#include <windows.h>
#include <desktop.h>
#include <window.h>
#include <external_window.h>
#include <non-standard/buildin/data/array.h>

#include <non-standard/stdio.h>
#include <non-standard/sys/spawn.h>
#include <stdlib.h>
#include <string.h>

#include "windows/launcher/launcher.h"
#include <filepicker.h>

window_definition_t** window_definitions = NULL;

void register_window(window_definition_t* def) {
    window_definitions = array_push(window_definitions, &def);
}

void launch_external(struct window_definition* def) {
    window_add_external(def->executable, NULL, def->x, def->y, def->width, def->height, def->name, def->bg_color);
}

void launch_external_ext(struct window_definition* def, const char* path) {
    window_add_external(def->executable, path, def->x, def->y, def->width, def->height, def->name, def->bg_color);
}

window_definition_t counter_def = {
    .name = "Counter",
    .register_window = launch_external,
    .executable = "desktop-counter",
    .x = 50, .y = 50, .width = 300, .height = 250, .bg_color = 0x334455
};

window_definition_t explorer_def = {
    .name = "Explorer",
    .register_window = launch_external,
    .executable = "desktop-explorer",
    .x = 100, .y = 100, .width = 400, .height = 300, .bg_color = 0x1a1a2e
};

window_definition_t taskmgr_def  = {
    .name = "Task Manager",
    .register_window = launch_external,
    .executable = "desktop-taskmgr",
    .x = 100, .y = 100, .width = 480, .height = 360, .bg_color = 0x1a1a2e
};

window_definition_t sysctl_def   = {
    .name = "System Control",
    .register_window = launch_external,
    .executable = "desktop-sysctl",
    .x = 60, .y = 60, .width = 280, .height = 260, .bg_color = 0x1a0a2e
};

window_definition_t netinfo_def  = {
    .name = "Network Info",
    .register_window = launch_external,
    .executable = "desktop-netinfo",
    .x = 60, .y = 60, .width = 400, .height = 320, .bg_color = 0x1a1a2e
};

window_definition_t edit_def     = {
    .name = "Editor",
    .register_window = launch_external,
    .executable = "desktop-edit",
    .x = 80, .y = 40, .width = 640, .height = 480, .bg_color = 0x1e1e2e
};

window_definition_t imgview_def  = {
    .name = "Image Viewer",
    .register_window = launch_external,
    .executable = "desktop-imgview",
    .x = 100, .y = 80, .width = 500, .height = 400, .bg_color = 0x000000
};

window_definition_t terminal_def = {
    .name = "Terminal",
    .register_window = launch_external,
    .executable = "desktop-terminal",
    .x = 60, .y = 60, .width = 640, .height = 400, .bg_color = 0x1a1a2e
};

window_definition_t service_def  = {
    .name = "Services",
    .register_window = launch_external,
    .executable = "desktop-service",
    .x = 60, .y = 60, .width = 400, .height = 360, .bg_color = 0x1a1a2e
};

void register_windows(void) {
    window_definitions = array_create(sizeof(window_definition_t*), 12);

    // TODO: load from config file?
    register_window(&launcher_definition);
    register_window(&counter_def);
    register_window(&explorer_def);
    register_window(&taskmgr_def);
    register_window(&sysctl_def);
    register_window(&netinfo_def);
    register_window(&edit_def);
    register_window(&imgview_def);
    register_window(&terminal_def);
    register_window(&service_def);

    desktop_register_file_assoc("fpic", &imgview_def, launch_external_ext);
    desktop_register_file_assoc("bmp",  &imgview_def, launch_external_ext);
    desktop_register_file_assoc("mbif", &imgview_def, launch_external_ext);

    desktop_register_file_assoc("txt",  &edit_def, launch_external_ext);
    desktop_register_file_assoc("conf",  &edit_def, launch_external_ext);
    desktop_register_file_assoc("md",  &edit_def, launch_external_ext);
    desktop_register_file_assoc("c",  &edit_def, launch_external_ext);
    desktop_register_file_assoc("asm",  &edit_def, launch_external_ext);
    desktop_register_file_assoc("msh",  &edit_def, launch_external_ext);
}

