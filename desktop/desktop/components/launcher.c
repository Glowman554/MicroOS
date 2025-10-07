#include <components/launcher.h>
#include <components/clickarea.h>
#include <components/mouse.h>
#include <launcher.h>
#include <fpic.h>
#include <render.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <buildin/path.h>

#define max_launchables 64

typedef struct launchable {
    click_area_t icon_area;
    const char* icon_path;
    const char* executable;
    char env[128];
    fpic_image_t* icon;
} launchable_t;

launchable_t launchables[max_launchables] = { 0 };

typedef struct shortcut {
    click_area_t icon_area;
    const char* icon_path;
    const char* file_path;
    fpic_image_t* icon;
} shortcut_t;

shortcut_t shortcuts[max_launchables] = { 0 };

extern config_node_t* config;

void init_program_launcher() {
    // TODO: load from config
    int current_launchable = 0;
    int current_shortcut = 0;
    // launchables[0].icon_path = "icons/doom.fpic";
    // launchables[0].executable = ROOT ":/bin/doom.elf";

    // launchables[1].icon_path = "icons/logo.fpic";
    // launchables[1].executable = ROOT ":/bin/window.elf";

    // launchables[2].icon_path = "icons/sysctl.fpic";
    // launchables[2].executable = ROOT ":/bin/sysctl.elf";

    for (int i = 0; config[i].key; i++) {
        if (strncmp(config[i].key, "program_", 8) == 0) {            
            launchables[current_launchable].icon_path = config[i].value;
            char* path = search_executable((char*) &config[i].key[8]);
            launchables[current_launchable].executable = path;
            sprintf(launchables[current_launchable].env, "%sopt/desktop/envs/%s.env", getenv("ROOT_FS"), &config[i].key[8]);
            current_launchable++;
        }
        if (strncmp(config[i].key, "shortcut_", 9) == 0) {            
            shortcuts[current_shortcut].icon_path = config[i].value;
            shortcuts[current_shortcut].file_path = &config[i].key[9];
            current_shortcut++;
        }
    }

    // printf("Found %d programs!\n", current_launchable);
    // printf("Found %d shortcuts!\n", current_shortcut);

    for (int i = 0; i < max_launchables; i++) {
        if (launchables[i].icon_path) {
            launchables[i].icon = load_fpic(launchables[i].icon_path);
        }
        if (shortcuts[i].icon_path) {
            shortcuts[i].icon = load_fpic(shortcuts[i].icon_path);
        }
    }
}

void draw_desktop_program_launcher() {
    int offset_x = 10;
    int offset_y = 10;
    for (int i = 0; i < max_launchables; i++) {
        if (launchables[i].icon) {
            if (offset_x + 64 > info.fb_width) {
                offset_x = 10;
                offset_y += 70;
            }

            draw_fpic_scaled(launchables[i].icon, offset_x, offset_y, 2);

            launchables[i].icon_area.x = offset_x;
            launchables[i].icon_area.y = offset_y;
            launchables[i].icon_area.width = 64;
            launchables[i].icon_area.height = 64;

            offset_x += 70;
        }
    }

    for (int i = 0; i < max_launchables; i++) {
        if (shortcuts[i].icon) {
            if (offset_x + 64 > info.fb_width) {
                offset_x = 10;
                offset_y += 70;
            }

            draw_fpic_scaled(shortcuts[i].icon, offset_x, offset_y, 2);

            shortcuts[i].icon_area.x = offset_x;
            shortcuts[i].icon_area.y = offset_y;
            shortcuts[i].icon_area.width = 64;
            shortcuts[i].icon_area.height = 64;

            offset_x += 70;
        }
    }
}


void launcher_tick() {
    for (int i = 0; i < max_launchables; i++) {
        if (launchables[i].icon) {
            if (check_click_area(&launchables[i].icon_area, &last_mouse)) {
                launch_child(launchables[i].executable, launchables[i].env, launchables[i].icon_path, NULL);
                break;
            }
        }

        if (shortcuts[i].icon) {
            if (check_click_area(&shortcuts[i].icon_area, &last_mouse)) {
                launch_program_for_file_extension(shortcuts[i].file_path);
                break;
            }
        }
    }   
}

char* get_file_extension(const char* filename) {
	char* chr_ptr = strchr(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}

void launch_program_for_file_extension(const char* file) {
    const char* extension = get_file_extension(file);
    if (strcmp("mex", extension) == 0) {
        // printf("Starting %s...\n", file);
        launch_child(file, "x", "icons/logo.fpic", NULL);
    } else {
        // printf("Starting program for %s (%s)...\n", extension, file);
        for (int i = 0; config[i].key; i++) {
            if (strncmp(config[i].key, "extension_", 10) == 0) {
                if (strcmp(&config[i].key[10], extension) == 0) {
                    // printf("Extension: %s\n", &config[i].key[10]);

                    char* path = search_executable((char*) config[i].value);

                    for (int i = 0; i < max_launchables; i++) {
                        if (launchables[i].icon) {
                            if (strcmp(launchables[i].executable, path) == 0) {
                                launch_child(launchables[i].executable, launchables[i].env, launchables[i].icon_path, file);
                            }
                        }
                    }

                    free(path);
                }
            }
        }
    }
}