#pragma once

#include <utils/multiboot.h>
#include <fs/devfs.h>


devfs_file_t* get_font_file(multiboot_module_t* font);