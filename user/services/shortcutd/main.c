#include <stdio.h>
#include <stdlib.h>
#include <non-standard/sys/message.h>
#include <non-standard/sys/spawn.h>

#define MAX_CMD 64

typedef struct {
    char key;
    char command[MAX_CMD];
} shortcut_launch_t;

int main(int argc, char* argv[], char* envp[]) {
    printf("shortcutd: Starting\n");

    shortcut_launch_t launch = {0};
    while (true) {
        uint32_t size = message_recv(TOPIC_SHORTCUT_LAUNCH, &launch, sizeof(launch));
        if (size > 0) {
            printf("shortcutd: Received launch request for key '%c'\n", launch.key);
            system(launch.command);
            printf("shortcutd: Executed command '%s' for key '%c'\n", launch.command, launch.key);
        } else {
            yield();
        }
    }
}