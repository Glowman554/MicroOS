#include <wm_client.h>
#include <wm_protocol.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/time.h>
#include <non-standard/sys/message.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BG_COLOR 0x0c1118
#define ROW_HEIGHT 22
#define TABLE_START_Y 46
#define BUTTON_W 56
#define BUTTON_H 18
#define POLL_INTERVAL_MS 80

#define OP_START   1
#define OP_STOP    2
#define OP_RESTART 3

const char* status_to_string(int status) {
    switch (status) {
        case SERVICE_STATUS_RUNNING: return "running";
        case SERVICE_STATUS_STOPPED: return "stopped";
        case SERVICE_STATUS_FAILED:  return "failed";
        default: return "unknown";
    }
}

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Services");
    wm_client_set_title_bar_color(&client, 0x20354a);
    wm_client_set_bg_color(&client, BG_COLOR);
    wm_client_set_realtime(&client, true);

    int cw = wm_client_width(&client);
    int ch = wm_client_height(&client);

    service_list_reply_t list;
    memset(&list, 0, sizeof(list));
    int has_list = 0;
    int waiting_for_list = 0;
    int waiting_for_op = 0;
    long next_poll_ms = 0;
    char status_text[96] = { 0 };

    wm_button_t refresh_btn;
    wm_btn_init(&refresh_btn, cw - 72, 4, 68, 18, "Refresh");
    refresh_btn.bg_color = 0x224455;
    refresh_btn.hover_color = 0x3377aa;

    int max_visible = (ch - TABLE_START_Y - 8) / ROW_HEIGHT;
    if (max_visible < 0) {
        max_visible = 0;
    }

    if (max_visible > MAX_SERVICES) {
        max_visible = MAX_SERVICES;
    }

    wm_button_t* start_btns = malloc(sizeof(wm_button_t) * max_visible);
    wm_button_t* stop_btns  = malloc(sizeof(wm_button_t) * max_visible);
    wm_button_t* restart_btns = malloc(sizeof(wm_button_t) * max_visible);
    memset(start_btns, 0, sizeof(wm_button_t) * max_visible);
    memset(stop_btns, 0, sizeof(wm_button_t) * max_visible);
    memset(restart_btns, 0, sizeof(wm_button_t) * max_visible);

    int dummy = 0;
    message_send(TOPIC_SERVICE_LIST, &dummy, sizeof(dummy));
    waiting_for_list = 1;
    next_poll_ms = time_ms() + POLL_INTERVAL_MS;
    strcpy(status_text, "Requesting service list...");

    int need_redraw = 1;

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
            if (evt.type == WM_EVENT_MOUSE_CLICK && evt.button == WM_MOUSE_BUTTON_LEFT) {
                if (wm_btn_hit(&refresh_btn, evt.x, evt.y) && !waiting_for_list && !waiting_for_op) {
                    message_send(TOPIC_SERVICE_LIST, &dummy, sizeof(dummy));
                    waiting_for_list = 1;
                    next_poll_ms = time_ms() + POLL_INTERVAL_MS;
                    strcpy(status_text, "Requesting service list...");
                    need_redraw = 1;
                }

                if (has_list) {
                    int count = list.count;
                    if (count > max_visible) {
                        count = max_visible;
                    }

                    for (int i = 0; i < count; i++) {
                        if (wm_btn_hit(&start_btns[i], evt.x, evt.y) && !waiting_for_list && !waiting_for_op) {
                            service_op_request_t req = { 0 };

                            size_t slen = strnlen(list.services[i].name, SERVICE_NAME_LEN - 1);
                            memcpy(req.name, list.services[i].name, slen);
                            req.name[slen] = 0;

                            message_send(TOPIC_SERVICE_START, &req, sizeof(req));
                            waiting_for_op = 1;
                            next_poll_ms = time_ms() + POLL_INTERVAL_MS;
                            sprintf(status_text, "Starting '%s'...", req.name);
                            need_redraw = 1;
                        }
                        
                        if (wm_btn_hit(&stop_btns[i], evt.x, evt.y) && !waiting_for_list && !waiting_for_op) {
                            service_op_request_t req = { 0 };
                            
                            size_t slen = strnlen(list.services[i].name, SERVICE_NAME_LEN - 1);
                            memcpy(req.name, list.services[i].name, slen);
                            req.name[slen] = 0;

                            message_send(TOPIC_SERVICE_STOP, &req, sizeof(req));
                            waiting_for_op = 1;
                            next_poll_ms = time_ms() + POLL_INTERVAL_MS;
                            sprintf(status_text, "Stopping '%s'...", req.name);
                            need_redraw = 1;
                        }

                        if (wm_btn_hit(&restart_btns[i], evt.x, evt.y) && !waiting_for_list && !waiting_for_op) {
                            service_op_request_t req = { 0 };
                            
                            size_t slen = strnlen(list.services[i].name, SERVICE_NAME_LEN - 1);
                            memcpy(req.name, list.services[i].name, slen);
                            req.name[slen] = 0;

                            message_send(TOPIC_SERVICE_RESTART, &req, sizeof(req));
                            waiting_for_op = 1;
                            next_poll_ms = time_ms() + POLL_INTERVAL_MS;
                            sprintf(status_text, "Restarting '%s'...", req.name);
                            need_redraw = 1;
                        }
                    }
                }
            }
            if (evt.type == WM_EVENT_MOUSE_MOVE) {
                int changed = 0;
                changed |= wm_btn_update_hover(&refresh_btn, evt.x, evt.y);
                if (has_list) {
                    int count = list.count;
                    if (count > max_visible) {
                        count = max_visible;
                    }

                    for (int i = 0; i < count; i++) {
                        changed |= wm_btn_update_hover(&start_btns[i], evt.x, evt.y);
                        changed |= wm_btn_update_hover(&stop_btns[i], evt.x, evt.y);
                        changed |= wm_btn_update_hover(&restart_btns[i], evt.x, evt.y);
                    }
                }
                if (changed) {
                    need_redraw = 1;
                }
            }
        }

        long now = time_ms();
        if (now >= next_poll_ms) {
            next_poll_ms = now + POLL_INTERVAL_MS;
            if (waiting_for_list) {
                service_list_reply_t reply = { 0 };
                if (message_recv(TOPIC_SERVICE_LIST_REPLY, &reply, sizeof(reply)) > 0) {
                    list = reply;
                    has_list = 1;
                    waiting_for_list = 0;
                    sprintf(status_text, "Loaded %d service(s)", list.count);
                    need_redraw = 1;

                    int count = list.count;
                    if (count > max_visible) {
                        count = max_visible;
                    }

                    int action_x = cw - (BUTTON_W * 3 + 10);
                    for (int i = 0; i < count; i++) {
                        int row_y = TABLE_START_Y + i * ROW_HEIGHT;
                        wm_btn_init(&start_btns[i], action_x, row_y, BUTTON_W, BUTTON_H, "Start");
                        start_btns[i].bg_color = 0x244426;
                        start_btns[i].hover_color = 0x2f6633;
                        wm_btn_init(&stop_btns[i], action_x + BUTTON_W + 3, row_y, BUTTON_W, BUTTON_H, "Stop");
                        stop_btns[i].bg_color = 0x553322;
                        stop_btns[i].hover_color = 0x885533;
                        wm_btn_init(&restart_btns[i], action_x + (BUTTON_W + 3) * 2, row_y, BUTTON_W, BUTTON_H, "Again");
                        restart_btns[i].bg_color = 0x334466;
                        restart_btns[i].hover_color = 0x4477aa;
                    }
                }
            }

            if (waiting_for_op) {
                service_op_reply_t op_reply = { 0 };
                if (message_recv(TOPIC_SERVICE_OP_REPLY, &op_reply, sizeof(op_reply)) > 0) {
                    waiting_for_op = 0;
                    if (op_reply.success) {
                        sprintf(status_text, "OK: %s", op_reply.message);
                    } else {
                        sprintf(status_text, "Error: %s", op_reply.message);
                    }

                    message_send(TOPIC_SERVICE_LIST, &dummy, sizeof(dummy));
                    waiting_for_list = 1;
                    next_poll_ms = now + POLL_INTERVAL_MS;
                    need_redraw = 1;
                }
            }
        }

        if (need_redraw) {
            int w = wm_client_width(&client);
            int h = wm_client_height(&client);
            wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

            wm_btn_draw(&refresh_btn, &client);

            wm_client_draw_line(&client, 0, 24, w, 24, 0x335577);
            wm_client_draw_string(&client, 4, 28, "Service", 0x99ccff, BG_COLOR);
            wm_client_draw_string(&client, 110, 28, "Status", 0x99ccff, BG_COLOR);
            wm_client_draw_string(&client, 190, 28, "PID", 0x99ccff, BG_COLOR);
            wm_client_draw_string(&client, 228, 28, "Retry", 0x99ccff, BG_COLOR);

            if (!has_list) {
                wm_client_draw_string(&client, 4, TABLE_START_Y, "No data loaded", 0xffcc88, BG_COLOR);
            } else {
                int count = list.count;
                if (count > max_visible) count = max_visible;
                for (int i = 0; i < count; i++) {
                    service_info_t* svc = &list.services[i];
                    int row_y = TABLE_START_Y + i * ROW_HEIGHT;
                    wm_client_draw_string(&client, 4, row_y, svc->name, 0xffffff, BG_COLOR);
                    wm_client_draw_string(&client, 110, row_y, status_to_string(svc->status), 0xffffff, BG_COLOR);

                    char num_buf[16] = { 0 };
                    sprintf(num_buf, "%d", svc->pid);
                    wm_client_draw_string(&client, 190, row_y, num_buf, 0xffffff, BG_COLOR);
                    sprintf(num_buf, "%d", svc->retry);
                    wm_client_draw_string(&client, 228, row_y, num_buf, 0xffffff, BG_COLOR);

                    wm_btn_draw(&start_btns[i], &client);
                    wm_btn_draw(&stop_btns[i], &client);
                    wm_btn_draw(&restart_btns[i], &client);
                }
            }

            wm_client_draw_line(&client, 0, h - 18, w, h - 18, 0x335577);
            wm_client_draw_string(&client, 4, h - 16, status_text, 0xaaddff, BG_COLOR);

            wm_client_flush(&client);
            need_redraw = 0;
        }

        yield();
    }

    free(start_btns);
    free(stop_btns);
    free(restart_btns);
    return 0;
}
