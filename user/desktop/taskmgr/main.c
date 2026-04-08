#include <wm_client.h>
#include <wm_protocol.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/raminfo.h>
#include <stdio.h>

#define BG_COLOR 0x1a1a2e

int main(int argc, char** argv) {
    wm_client_t client;
    wm_client_init(&client);

    wm_client_set_title(&client, "Task Manager");
    wm_client_set_title_bar_color(&client, 0x884422);
    wm_client_set_bg_color(&client, BG_COLOR);
    wm_client_set_realtime(&client, true);

    task_list_t tasks[64] = { 0 };
    int count = 0;

    while (!wm_client_should_close(&client)) {
        wm_event_t evt;
        while (wm_client_poll_event(&client, &evt)) {
        }

        int w = wm_client_width(&client);
        int h = wm_client_height(&client);
        wm_client_fill_rect(&client, 0, 0, w, h, BG_COLOR);

        wm_client_draw_string(&client, 4, 2, "PID", 0xaaaaff, BG_COLOR);
        wm_client_draw_string(&client, 60, 2, "TERM", 0xaaaaff, BG_COLOR);
        wm_client_draw_string(&client, 120, 2, "NAME", 0xaaaaff, BG_COLOR);
        wm_client_draw_line(&client, 0, 18, w, 18, 0x444444);

        count = get_task_list(tasks, 64);

        int y = 20;
        for (int i = 0; i < count && y + 16 < h - 30; i++) {
            char pid_str[16];
            sprintf(pid_str, "%d", tasks[i].pid);
            char term_str[16];
            sprintf(term_str, "%d", tasks[i].term);

            wm_client_draw_string(&client, 4, y, pid_str, 0xffffff, BG_COLOR);
            wm_client_draw_string(&client, 60, y, term_str, 0xffffff, BG_COLOR);
            wm_client_draw_string(&client, 120, y, tasks[i].name, 0xffffff, BG_COLOR);


            y += 18;
        }

        uint32_t mem_free, mem_used;
        raminfo(&mem_free, &mem_used);
        uint32_t total = mem_free + mem_used;

        int bar_y = h - 24;
        int bar_w = w - 8;
        wm_client_fill_rect(&client, 4, bar_y, bar_w, 16, 0x333333);

        if (total > 0) {
            int used_w = (int)((uint64_t)mem_used * bar_w / total);
            wm_client_fill_rect(&client, 4, bar_y, used_w, 16, 0x44aa44);
        }

        char mem_str[64];
        sprintf(mem_str, "Memory: %d / %d KB", mem_used / 1024, total / 1024);
        wm_client_draw_string(&client, 8, bar_y, mem_str, 0xffffff, 0x333333);

        wm_client_flush(&client);
        yield();
    }

    return 0;
}
