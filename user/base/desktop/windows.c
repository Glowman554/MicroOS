#include <windows.h>
#include <desktop.h>
#include <non-standard/buildin/data/array.h>

#include <non-standard/stdio.h>
#include <stdlib.h>
#include <load.h>

#include "windows/launcher/launcher.h"
#include "windows/counter/counter.h"
#include "windows/explorer/explorer.h"
#include "windows/taskmgr/taskmgr.h"
#include "windows/sysctl/sysctl.h"
#include "windows/netinfo/netinfo.h"
#include "windows/imgview/imgview.h"
#include "windows/edit/edit.h"
#include "windows/terminal/terminal.h"
#include "windows/service/service.h"

window_definition_t** window_definitions = NULL;

void load_extension(const char* s) {
    FILE* file = fopen(s, "r");
	if (!file) {
		printf("Could not open %s\n", s);
		abort();
	}

	fsize(file, size);
	void* buf = malloc(size);
	fread(buf, 1, size, file);
	fclose(file);
    

    loaded_object_t obj = load(buf, size);
    
    void (*init)() = symbol(&obj, "init");
	if (!init) {
		printf("Could not find init symbol\n");
        abort();
	}

    init();
}

void register_window(window_definition_t* def) {
    window_definitions = array_push(window_definitions, &def);
}

void register_windows(void) {
    window_definitions = array_create(sizeof(window_definition_t*), 8);

    register_window(&launcher_definition);
    register_window(&counter_definition);
    register_window(&explorer_definition);
    register_window(&taskmgr_definition);
    register_window(&sysctl_definition);
    register_window(&netinfo_definition);
    register_window(&edit_definition);
    register_window(&imgview_definition);
    register_window(&terminal_definition);
    register_window(&service_window_definition);

    desktop_register_file_assoc("fpic", imgview_open);
    desktop_register_file_assoc("bmp",  imgview_open);
    desktop_register_file_assoc("mbif", imgview_open);
    desktop_register_file_assoc("o", load_extension);
}

