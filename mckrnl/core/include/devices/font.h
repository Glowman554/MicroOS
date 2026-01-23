#pragma once

#include <utils/multiboot.h>
#include <fs/devfs.h>


devfs_file_t* get_font_file(int font_size, void* font_pointer);
