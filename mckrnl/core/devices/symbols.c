#include <devices/symbols.h>
#include <scheduler/scheduler.h>

#include <assert.h>
#include <string.h>

#include <config.h>

#ifdef SYMBOLS_FILE
void symbols_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    task_t* self = get_self();
    assert(size + offset <= self->num_symbols * sizeof(symbols_t));
    memcpy(buf, (void*) self->symbols + offset, size);
}


void symbols_file_prepare(struct devfs_file* dfile, file_t* file) {
    task_t* self = get_self();
    file->size = self->num_symbols * sizeof(symbols_t);
}

char* symbols_file_name(devfs_file_t* file) {
	return "symbols";
}

devfs_file_t symbols_file = {
	.read = symbols_file_read,
    .prepare = symbols_file_prepare,
	.name = symbols_file_name
};
#endif