#include <devices/shortcut.h>
#include <memory/heap.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

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
        debugf(SPAM, "          Launcher for '%c' is '%s'", shortcuts[i].key, shortcuts[i].launcher);
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
            char* argv[] = {
                shortcuts[i].launcher,
                shortcuts[i].command,
                NULL
            };

            char* envp[] = {
                "SHORTCUT=yes",
                NULL
            };

            // TODO: select proper terminal
            int pid = load_executable(1, shortcuts[i].launcher, argv, envp);
            if (pid == -1) {
                debugf(ERROR, "Failed to execute shortcut '%c': Failed to load executable '%s'", shortcuts[i].key, shortcuts[i].launcher);
                return false;
            }
            task_t* task = get_task_by_pid(pid);
            strcpy(task->pwd, shortcuts[i].pwd);

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