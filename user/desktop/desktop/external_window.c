#include <external_window.h>
#include <window.h>
#include <framebuffer.h>
#include <wm_protocol.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <non-standard/stdio.h>
#include <non-standard/buildin/path.h>
#include <non-standard/sys/mmap.h>
#include <non-standard/sys/spawn.h>

bool slot_used[WM_MAX_SLOTS] = { false };

int external_window_alloc_slot(void) {
    for (int i = 0; i < WM_MAX_SLOTS; i++) {
        if (!slot_used[i]) {
            slot_used[i] = true;
            return i;
        }
    }
    return -1;
}

void external_window_free_slot(int slot) {
    if (slot >= 0 && slot < WM_MAX_SLOTS) {
        slot_used[slot] = false;
    }
}

void map_shm_pages(int slot, int pid, int content_w, int content_h) {
    uintptr_t base = (uintptr_t)WM_SHM_ADDR(slot);

    mmmap((void*)base, (void*)base, pid);

    int pixel_bytes = content_w * content_h * 4;
    int pixel_pages = (pixel_bytes + WM_PAGE_SIZE - 1) / WM_PAGE_SIZE;

    for (int i = 0; i < pixel_pages; i++) {
        uintptr_t addr = base + WM_PIXELS_OFFSET + i * WM_PAGE_SIZE;
        mmmap((void*)addr, (void*)addr, pid);
    }
}

void setup_shm_control(external_state_t* est, window_instance_t* w, int slot, int content_w, int content_h) {
    wm_shared_t* ctl = WM_SHM_CONTROL(slot);
    memset(ctl, 0, sizeof(wm_shared_t));
    ctl->width = content_w;
    ctl->height = content_h;
    ctl->event_write = 0;
    ctl->event_read = 0;
    ctl->dirty = 0;
    ctl->alive = 0;
    ctl->close_requested = 0;
    memcpy(ctl->title, w->title, 64);
    ctl->bg_color = w->bg_color;
    ctl->title_bar_color = w->title_bar_color;

    est->control = ctl;
    est->pixels = WM_SHM_PIXELS(slot);
}

void wait_for_client(wm_shared_t* ctl) {
    ctl->state = WM_STATE_READY;

    int timeout = 500;
    while (ctl->state != WM_STATE_ACK && timeout > 0) {
        yield();
        timeout--;
    }

    if (ctl->state == WM_STATE_ACK) {
        ctl->state = WM_STATE_CONNECTED;
    }
}

void external_window_init(window_instance_t* w) {
    external_state_t* est = (external_state_t*)w->state;

    int slot = est->slot;
    int content_w = w->width;
    int content_h = w->height - TITLE_BAR_HEIGHT;

    if (content_w > WM_MAX_PIXEL_WIDTH) {
        content_w = WM_MAX_PIXEL_WIDTH;
    }
    if (content_h > WM_MAX_PIXEL_HEIGHT) {
        content_h = WM_MAX_PIXEL_HEIGHT;
    }
    if (content_w < 1) {
        content_w = 1;
    }
    if (content_h < 1) {
        content_h = 1;
    }

    char* exec = search_executable((char*) est->executable);
    if (!exec) {
        printf("desktop: failed to resolve %s\n", est->executable);
        return;
    }

    const char* argv[] = { est->executable, est->launch_file, NULL };

    char slot_str[16];
    sprintf(slot_str, "WMS=%d", slot);

    int envc = 0;
    for (char** e = global_envp; *e; e++) {
        envc++;
    }
    const char* envp[envc + 2];
    for (int i = 0; i < envc; i++) {
        envp[i] = global_envp[i];
    }
    envp[envc] = slot_str;
    envp[envc + 1] = NULL;

    int pid = spawn(exec, argv, envp);
    if (pid < 0) {
        free(exec);
        printf("desktop: failed to spawn %s\n", est->executable);
        return;
    }

    est->pid = pid;
    free(exec);
    w->child_pid = pid;

    map_shm_pages(slot, pid, content_w, content_h);
    setup_shm_control(est, w, slot, content_w, content_h);
    wait_for_client(est->control);

    w->is_external = true;
    w->shm_slot = slot;
    w->shm_base = (void*)WM_SHM_ADDR(slot);
}

void external_window_init_spawned(window_instance_t* w) {
    external_state_t* est = (external_state_t*)w->state;

    int slot = est->slot;
    int pid = est->pid;
    int content_w = w->width;
    int content_h = w->height - TITLE_BAR_HEIGHT;

    if (content_w > WM_MAX_PIXEL_WIDTH) {
        content_w = WM_MAX_PIXEL_WIDTH;
    }
    if (content_h > WM_MAX_PIXEL_HEIGHT) {
        content_h = WM_MAX_PIXEL_HEIGHT;
    }
    if (content_w < 1) {
        content_w = 1;
    }
    if (content_h < 1) {
        content_h = 1;
    }

    w->child_pid = pid;

    map_shm_pages(slot, pid, content_w, content_h);
    setup_shm_control(est, w, slot, content_w, content_h);
    wait_for_client(est->control);

    w->is_external = true;
    w->shm_slot = slot;
    w->shm_base = (void*)WM_SHM_ADDR(slot);
}

void external_window_send_event(window_instance_t* w, event_t* event) {
    external_state_t* est = (external_state_t*)w->state;
    if (!est || !est->control) {
        return;
    }

    if (event->type == EVENT_NONE) { // event is only to tick the local update
        return;
    }

    wm_shared_t* ctl = est->control;

    if (ctl->event_write - ctl->event_read >= WM_MAX_EVENTS) {
        return;
    }

    int idx = ctl->event_write % WM_MAX_EVENTS;
    ctl->events[idx].type = event->type;
    ctl->events[idx].x = event->x;
    ctl->events[idx].y = event->y - TITLE_BAR_HEIGHT;
    ctl->events[idx].button = event->button;
    ctl->events[idx].key = event->key;
    ctl->events[idx].arrow = event->arrow;
    ctl->event_write++;
}

void external_window_update(window_instance_t* w, event_t* event) {
    external_state_t* est = (external_state_t*)w->state;
    if (!est || !est->control) {
        return;
    }

    wm_shared_t* ctl = est->control;

    if (!get_proc_info(est->pid)) {
        w->is_dirty = true;
        return;
    }

    if (ctl->title[0] != '\0') {
        memcpy(w->title, ctl->title, 64);
    }
    w->bg_color = ctl->bg_color;
    w->title_bar_color = ctl->title_bar_color;
    external_window_send_event(w, event);

    if (ctl->dirty) {
        w->is_dirty = true;
        ctl->dirty = 0;
    }
}

void external_window_draw(window_instance_t* w) {
    external_state_t* est = (external_state_t*)w->state;
    if (!est || !est->pixels) {
        return;
    }

    wm_shared_t* ctl = est->control;
    int content_w = ctl->width;
    int content_h = ctl->height;
    int win_x = w->x;
    int win_y = w->y + TITLE_BAR_HEIGHT;

    for (int j = 0; j < content_h; j++) {
        for (int i = 0; i < content_w; i++) {
            uint32_t color = est->pixels[j * content_w + i];
            uint32_t fb_x = win_x + i;
            uint32_t fb_y = win_y + j;
            if (fb_x < framebuffer.fb_width && fb_y < framebuffer.fb_height) {
                *(uint32_t*)(framebuffer.fb_addr + (fb_x * 4) + (fb_y * framebuffer.fb_pitch)) = color;
            }
        }
    }
}

void external_window_cleanup(window_instance_t* w) {
    external_state_t* est = (external_state_t*)w->state;
    if (!est) {
        return;
    }

    if (est->control) {
        est->control->close_requested = 1;
    }

    for (int i = 0; i < 50; i++) {
        if (!get_proc_info(est->pid)) {
            break;
        }
        yield();
    }

    if (get_proc_info(est->pid)) {
        kill(est->pid);
    }

    external_window_free_slot(est->slot);
    free(est);
    w->state = NULL;
    w->is_external = false;
}

void window_add_external(const char* executable, const char* launch_file, int x, int y, int width, int height, const char* title, uint32_t bg_color) {
    int slot = external_window_alloc_slot();
    if (slot < 0) {
        printf("desktop: no free SHM slots\n");
        return;
    }

    external_state_t* est = malloc(sizeof(external_state_t));
    memset(est, 0, sizeof(external_state_t));
    est->executable = executable;
    est->launch_file = launch_file;
    est->slot = slot;

    window_add_with_state(x, y, width, height, title, bg_color,
                          est,
                          external_window_init,
                          external_window_update,
                          external_window_draw,
                          external_window_cleanup);
}
