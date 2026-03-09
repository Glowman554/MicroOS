#include <windows.h>
#include <desktop.h>
#include <buildin/array.h>

#include "windows/launcher/launcher.h"
#include "windows/counter/counter.h"
#include "windows/text/text.h"
#include "windows/explorer/explorer.h"
#include "windows/taskmgr/taskmgr.h"
#include "windows/sysctl/sysctl.h"
#include "windows/netinfo/netinfo.h"
#include "windows/imgview/imgview.h"

window_definition_t** window_definitions = NULL;

void register_windows(void) {
    window_definitions = array_create(sizeof(window_definition_t*), 8);

    window_definition_t* def;

    def = &launcher_definition;
    window_definitions = array_push(window_definitions, &def);

    def = &counter_definition;
    window_definitions = array_push(window_definitions, &def);

    def = &text_definition;
    window_definitions = array_push(window_definitions, &def);

    def = &explorer_definition;
    window_definitions = array_push(window_definitions, &def);

    def = &taskmgr_definition;
    window_definitions = array_push(window_definitions, &def);

    def = &sysctl_definition;
    window_definitions = array_push(window_definitions, &def);

    def = &netinfo_definition;
    window_definitions = array_push(window_definitions, &def);

    desktop_register_file_assoc("fpic", imgview_open);
    desktop_register_file_assoc("bmp",  imgview_open);
    desktop_register_file_assoc("mbif", imgview_open);
}

