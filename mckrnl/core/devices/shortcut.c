#include <devices/shortcut.h>
#include <memory/heap.h>
#include <scheduler/scheduler.h>
#include <scheduler/message.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct {
    char key;
    char command[MAX_CMD];
} shortcut_launch_t;

shortcut_t* shortcuts = NULL;
int shortcut_count = 0;

void shortcut_file_write(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    assert(size % sizeof(shortcut_t) == 0);
    int count = size / sizeof(shortcut_t);
    assert(offset == 0);

    shortcuts = krealloc(shortcuts, count * sizeof(shortcut_t));
    memcpy(shortcuts, buf, count * sizeof(shortcut_t));
    shortcut_count = count;

    file->size = size;

    for (int i = 0; i < shortcut_count; i++) {
        debugf(SPAM, "shortcut: Registered shortcut '%c' -> '%s'", shortcuts[i].key, shortcuts[i].command);
    }

}

void shortcut_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    assert(size + offset <= file->size);
    memcpy(buf, (void*)shortcuts + offset, size);
}

void shortcut_file_prepare(struct devfs_file* dfile, file_t* file) {
    file->size = shortcut_count * sizeof(shortcut_t);
}

char* shortcut_file_name(devfs_file_t* file) {
    return "shortcut";
}

bool execute_shortcut(char key) {
    for (int i = 0; i < shortcut_count; i++) {
        if (shortcuts[i].key == key) {
            debugf(SPAM, "Executing shortcut '%c' -> '%s'", shortcuts[i].key, shortcuts[i].command);
            
            shortcut_launch_t launch = { 0 };
            launch.key = shortcuts[i].key;
            strcpy(launch.command, shortcuts[i].command);
            message_send(TOPIC_SHORTCUT_LAUNCH, &launch, sizeof(launch));

            return true;
        }
    }

    debugf(SPAM, "No shortcut found for key '%c'", key);
    return false;
}

devfs_file_t shortcut_file = {
    .name = shortcut_file_name,
    .prepare = shortcut_file_prepare,
    .read = shortcut_file_read,
    .write = shortcut_file_write
};