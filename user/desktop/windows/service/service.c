#include "service.h"

#include <window.h>
#include <window_helpers.h>
#include <button.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <non-standard/sys/time.h>

#define ROW_HEIGHT 22
#define TABLE_START_Y 46
#define BUTTON_W 56
#define BUTTON_H 18
#define POLL_INTERVAL_MS 80

#define OP_START   1
#define OP_STOP    2
#define OP_RESTART 3

static int service_safe_count(const service_window_state_t* state) {
    int count = state->list.count;
    if (count < 0) {
        return 0;
    }
    if (count > MAX_SERVICES) {
        return MAX_SERVICES;
    }
    return count;
}

static void service_request_list(service_window_state_t* state, window_instance_t* w) {
    int dummy = 0;
    message_send(TOPIC_SERVICE_LIST, &dummy, sizeof(dummy));
    state->waiting_for_list = 1;
    state->next_poll_ms = time_ms() + POLL_INTERVAL_MS;
    strcpy(state->status_text, "Requesting service list...");
    w->is_realtime = true;
}

static void service_request_op(service_window_state_t* state, window_instance_t* w, int service_idx, int op) {
    int service_count = service_safe_count(state);
    if (!state->has_list || service_idx < 0 || service_idx >= service_count) {
        return;
    }

    service_op_request_t req = { 0 };
    memset(&req, 0, sizeof(req));
    size_t slen = strnlen(state->list.services[service_idx].name, SERVICE_NAME_LEN - 1);
    memcpy(req.name, state->list.services[service_idx].name, slen);
    req.name[slen] = 0;

    uint32_t topic = TOPIC_SERVICE_START;
    const char* op_name = "start";
    if (op == OP_STOP) {
        topic = TOPIC_SERVICE_STOP;
        op_name = "stop";
    } else if (op == OP_RESTART) {
        topic = TOPIC_SERVICE_RESTART;
        op_name = "restart";
    }

    message_send(topic, &req, sizeof(req));
    state->waiting_for_op = 1;
    state->next_poll_ms = time_ms() + POLL_INTERVAL_MS;

    sprintf(state->status_text, "Requesting %s for '%s'...", op_name, req.name);
    w->is_realtime = true;
}

static void on_service_refresh(window_instance_t* w, void* userdata) {
    service_window_state_t* state = (service_window_state_t*)w->state;
    if (state->waiting_for_list || state->waiting_for_op) {
        return;
    }
    service_request_list(state, w);
}

static void on_service_start(window_instance_t* w, void* userdata) {
    service_window_state_t* state = (service_window_state_t*)w->state;
    if (state->waiting_for_list || state->waiting_for_op) {
        return;
    }
    int idx = (int)(intptr_t)userdata;
    service_request_op(state, w, idx, OP_START);
}

static void on_service_stop(window_instance_t* w, void* userdata) {
    service_window_state_t* state = (service_window_state_t*)w->state;
    if (state->waiting_for_list || state->waiting_for_op) {
        return;
    }
    int idx = (int)(intptr_t)userdata;
    service_request_op(state, w, idx, OP_STOP);
}

static void on_service_restart(window_instance_t* w, void* userdata) {
    service_window_state_t* state = (service_window_state_t*)w->state;
    if (state->waiting_for_list || state->waiting_for_op) {
        return;
    }
    int idx = (int)(intptr_t)userdata;
    service_request_op(state, w, idx, OP_RESTART);
}

static void service_rebuild_buttons(service_window_state_t* state, window_instance_t* w) {
    int visible_rows = (w->height - TITLE_BAR_HEIGHT - TABLE_START_Y - 8) / ROW_HEIGHT;
    if (visible_rows < 0) {
        visible_rows = 0;
    }

    int count = service_safe_count(state);
    if (count > visible_rows) {
        count = visible_rows;
    }

    int action_x = w->width - (BUTTON_W * 3 + 10);
    for (int i = 0; i < count; i++) {
        int row_y = TABLE_START_Y + i * ROW_HEIGHT;

        button_init(&state->start_buttons[i], action_x, row_y, BUTTON_W, BUTTON_H, "Start", on_service_start, (void*)(intptr_t)i);
        state->start_buttons[i].bg_color = 0x244426;
        state->start_buttons[i].hover_color = 0x2f6633;

        button_init(&state->stop_buttons[i], action_x + BUTTON_W + 3, row_y, BUTTON_W, BUTTON_H, "Stop", on_service_stop, (void*)(intptr_t)i);
        state->stop_buttons[i].bg_color = 0x553322;
        state->stop_buttons[i].hover_color = 0x885533;

        button_init(&state->restart_buttons[i], action_x + (BUTTON_W + 3) * 2, row_y, BUTTON_W, BUTTON_H, "Again", on_service_restart, (void*)(intptr_t)i);
        state->restart_buttons[i].bg_color = 0x334466;
        state->restart_buttons[i].hover_color = 0x4477aa;
    }
}

static const char* service_status_to_string(int status) {
    switch (status) {
        case SERVICE_STATUS_RUNNING:
            return "running";
        case SERVICE_STATUS_STOPPED:
            return "stopped";
        case SERVICE_STATUS_FAILED:
            return "failed";
        default:
            return "unknown";
    }
}

static void service_poll_replies(service_window_state_t* state, window_instance_t* w) {
    long now = time_ms();
    if (now < state->next_poll_ms) {
        return;
    }
    state->next_poll_ms = now + POLL_INTERVAL_MS;

    if (state->waiting_for_list) {
        service_list_reply_t reply = { 0 };
        memset(&reply, 0, sizeof(reply));
        if (message_recv(TOPIC_SERVICE_LIST_REPLY, &reply, sizeof(reply)) > 0) {
            state->list = reply;
            state->has_list = 1;
            state->waiting_for_list = 0;
            service_rebuild_buttons(state, w);
            sprintf(state->status_text, "Loaded %d service(s)", service_safe_count(state));
            w->is_dirty = true;
        }
    }

    if (state->waiting_for_op) {
        service_op_reply_t op_reply = { 0 };
        memset(&op_reply, 0, sizeof(op_reply));
        if (message_recv(TOPIC_SERVICE_OP_REPLY, &op_reply, sizeof(op_reply)) > 0) {
            state->waiting_for_op = 0;
            if (op_reply.success) {
                sprintf(state->status_text, "OK: %s", op_reply.message);
            } else {
                sprintf(state->status_text, "Error: %s", op_reply.message);
            }
            w->is_dirty = true;

            service_request_list(state, w);
        }
    }

    if (!state->waiting_for_list && !state->waiting_for_op) {
        w->is_realtime = false;
    }
}

void service_window_init(window_instance_t* w) {
    service_window_state_t* state = malloc(sizeof(service_window_state_t));
    memset(state, 0, sizeof(service_window_state_t));

    button_init(&state->refresh_btn, w->width - 72, 4, 68, 18, "Refresh", on_service_refresh, NULL);
    state->refresh_btn.bg_color = 0x224455;
    state->refresh_btn.hover_color = 0x3377aa;

    w->state = state;
    w->title_bar_color = 0x20354a;
    w->is_dirty = true;

    service_request_list(state, w);
}

void service_window_update(window_instance_t* w, event_t* event) {
    service_window_state_t* state = (service_window_state_t*)w->state;

    button_handle_event(&state->refresh_btn, w, event);

    if (state->has_list) {
        service_rebuild_buttons(state, w);

        int visible_rows = (w->height - TITLE_BAR_HEIGHT - TABLE_START_Y - 8) / ROW_HEIGHT;
        if (visible_rows < 0) {
            visible_rows = 0;
        }

        int count = service_safe_count(state);
        if (count > visible_rows) {
            count = visible_rows;
        }

        for (int i = 0; i < count; i++) {
            button_handle_event(&state->start_buttons[i], w, event);
            button_handle_event(&state->stop_buttons[i], w, event);
            button_handle_event(&state->restart_buttons[i], w, event);
        }
    }

    if (event->type == EVENT_NONE && (state->waiting_for_list || state->waiting_for_op)) {
        service_poll_replies(state, w);
    }
}

void service_window_draw(window_instance_t* w) {
    service_window_state_t* state = (service_window_state_t*)w->state;

    button_draw(&state->refresh_btn, w);

    window_draw_line(w, 0, 24, w->width, 24, 0x335577);
    window_draw_string(w, 4, 28, "Service", 0x99ccff);
    window_draw_string(w, 110, 28, "Status", 0x99ccff);
    window_draw_string(w, 190, 28, "PID", 0x99ccff);
    window_draw_string(w, 228, 28, "Retry", 0x99ccff);

    int visible_rows = (w->height - TITLE_BAR_HEIGHT - TABLE_START_Y - 8) / ROW_HEIGHT;
    if (visible_rows < 0) {
        visible_rows = 0;
    }

    if (!state->has_list) {
        window_draw_string(w, 4, TABLE_START_Y, "No data loaded", 0xffcc88);
    } else {
        int count = service_safe_count(state);
        if (count > visible_rows) {
            count = visible_rows;
        }

        for (int i = 0; i < count; i++) {
            service_info_t* svc = &state->list.services[i];
            int row_y = TABLE_START_Y + i * ROW_HEIGHT;

            window_draw_string(w, 4, row_y, svc->name, 0xffffff);
            window_draw_string(w, 110, row_y, service_status_to_string(svc->status), 0xffffff);

            char num_buf[16] = { 0 };
            sprintf(num_buf, "%d", svc->pid);
            window_draw_string(w, 190, row_y, num_buf, 0xffffff);

            sprintf(num_buf, "%d", svc->retry);
            window_draw_string(w, 228, row_y, num_buf, 0xffffff);

            button_draw(&state->start_buttons[i], w);
            button_draw(&state->stop_buttons[i], w);
            button_draw(&state->restart_buttons[i], w);
        }

        if (service_safe_count(state) > count) {
            char more_buf[48] = { 0 };
            sprintf(more_buf, "%d more service(s) not shown", service_safe_count(state) - count);
            window_draw_string(w, 4, w->height - TITLE_BAR_HEIGHT - 34, more_buf, 0xcccc88);
        }
    }

    window_draw_line(w, 0, w->height - TITLE_BAR_HEIGHT - 18, w->width, w->height - TITLE_BAR_HEIGHT - 18, 0x335577);
    window_draw_string(w, 4, w->height - TITLE_BAR_HEIGHT - 16, state->status_text, 0xaaddff);
}

void service_window_cleanup(window_instance_t* w) {
    if (w->state) {
        free(w->state);
        w->state = NULL;
    }
}

window_definition_t service_window_definition = {
    .name = "Services",
    .register_window = register_service_window,
};

void register_service_window(void) {
    window_add(90, 90, 500, 360, "Services", 0x0c1118, service_window_init, service_window_update, service_window_draw, service_window_cleanup);
}