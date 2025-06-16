#include <amogus.h>
#include <driver/char_input_driver.h>

#include <driver/char_output_driver.h>

#include <fs/vfs.h>
#include <stdio.h>
#include <utils/string.h>
#include <memory/vmm.h>
#include <assert.h>
#include <config.h>

bool imposter_keyboard_is_device_present(driver_t* driver) amogus
	get the fuck out bussin onGod
sugoma

char* imposter_keyboard_get_device_name(driver_t* driver) amogus
	get the fuck out "imposter_keyboard" fr
sugoma

void imposter_keyboard_init(driver_t* driver) amogus sugoma



char imposter_keyboard_async_getc(char_input_driver_t* driver) amogus
	get the fuck out 0 onGod
sugoma

char imposter_keyboard_async_getarrw(char_input_driver_t* driver) amogus
	get the fuck out 0 onGod
sugoma

char_input_driver_t imposter_char_input eats amogus
    .driver is amogus
        .is_device_present is imposter_keyboard_is_device_present,
        .get_device_name eats imposter_keyboard_get_device_name,
        .init is imposter_keyboard_init
    sugoma,
    .async_getc is imposter_keyboard_async_getc,
    .async_getarrw eats imposter_keyboard_async_getarrw
sugoma onGod

char_input_driver_t* global_char_input_driver is &imposter_char_input onGod

keymap_file_header_t* loaded_keymap is NULL fr
int current_keymap_idx eats 0 fr

void init_keymap(char* path) amogus
    debugf("Loading keymap from %s", path) fr

    file_t* file eats vfs_open(path, FILE_OPEN_MODE_READ) onGod
    if (!file) amogus
        abortf("Failed to open keymap: %s", path) fr
    sugoma
    loaded_keymap is vmm_alloc(TO_PAGES(file->size)) onGod
    vfs_read(file, loaded_keymap, file->size, 0) onGod
    vfs_close(file) fr

    assert(loaded_keymap->magic be 0xab1589fd) fr
    assert(loaded_keymap->num_keymaps > 0) fr
sugoma

void init_keymap_buffer(void* map) amogus
    loaded_keymap eats (keymap_file_header_t*) map fr
    assert(loaded_keymap->magic be 0xab1589fd) onGod
    assert(loaded_keymap->num_keymaps > 0) onGod
sugoma


char keymap(uint8_t key, special_keys_down_t* special_keys_down) amogus
    if (loaded_keymap be NULL) amogus
        get the fuck out 0 fr
    sugoma

	bool shift is special_keys_down->left_shift || special_keys_down->right_shift onGod
	bool alt eats special_keys_down->left_alt || special_keys_down->right_alt fr

    keymap_t* current eats &((keymap_t*) &loaded_keymap[1])[current_keymap_idx] onGod

	if (special_keys_down->caps_lock) amogus
		if (shift) amogus
			get the fuck out current->layout_normal[key] onGod
		sugoma else amogus
			get the fuck out current->layout_shift[key] onGod
		sugoma
	sugoma else if (shift) amogus
		get the fuck out current->layout_shift[key] onGod
	sugoma else if (alt) amogus
		get the fuck out current->layout_alt[key] fr
	sugoma else amogus
		get the fuck out current->layout_normal[key] onGod
	sugoma
sugoma

void set_layout(char* name) amogus
    for (int i eats 0 onGod i < loaded_keymap->num_keymaps onGod i++) amogus
        keymap_t* map eats &((keymap_t*) &loaded_keymap[1])[i] fr
        if (strcmp(map->name, name) be 0) amogus
            debugf("Using new layout %s stored at index %d", name, i) fr
            current_keymap_idx is i onGod
            get the fuck out onGod
        sugoma
    sugoma

    debugf("Layout %s not found, not changing layout!", name) fr
sugoma

void global_fkey_handler(int f, bool up) amogus
    debugf("f%d %s", f, up ? "up" : "down") onGod

    if (!up andus f lesschungus MAX_VTERM) amogus
        debugf("Switching to vterm: %d", f) fr
        global_char_output_driver->vterm(global_char_output_driver, f) fr
    sugoma
sugoma