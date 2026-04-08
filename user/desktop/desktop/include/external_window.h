#pragma once

#include <types.h>
#include <wm_protocol.h>

// State struct stored in window_instance_t.state for external windows
typedef struct {
    const char* executable;
    int slot;
    int pid;
    wm_shared_t* control;
    uint32_t* pixels;
    const char* launch_file; // for file associations
} external_state_t;

// Open an external window by spawning a child process.
// executable: path to the .mex file
// x, y, width, height: initial window geometry
// title: initial window title
// bg_color: background color
void window_add_external(const char* executable, const char* launch_file, int x, int y, int width, int height,
                         const char* title, uint32_t bg_color);

// Send an event to an external window's shared memory event queue.
void external_window_send_event(window_instance_t* w, event_t* event);

// Draw callback for external windows: copies pixels from SHM into the framebuffer.
void external_window_draw(window_instance_t* w);

// Update callback for external windows: checks client state, sends events.
void external_window_update(window_instance_t* w, event_t* event);

// Cleanup callback for external windows: kills child, unmaps SHM.
void external_window_cleanup(window_instance_t* w);

// Init callback for external windows (called after window_add sets up the struct).
void external_window_init(window_instance_t* w);

// Init callback for external windows where the child is already spawned.
// Used when opening files via file associations.
void external_window_init_spawned(window_instance_t* w);

// Allocate the next free SHM slot. Returns -1 if none available.
int external_window_alloc_slot(void);

// Free a previously allocated SHM slot.
void external_window_free_slot(int slot);
