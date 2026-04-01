#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <non-standard/sys/message.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/env.h>

#define WAIT_RETRIES 15
#define WAIT_TIME_MS 200

static int wait_for_list_reply(service_list_reply_t* reply) {
    for (int i = 0; i < WAIT_RETRIES; i++) {
        uint32_t size = message_recv(TOPIC_SERVICE_LIST_REPLY, reply, sizeof(*reply));
        if (size > 0) return 1;
        set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*) WAIT_TIME_MS);
        yield();
    }
    return 0;
}

static int wait_for_op_reply(service_op_reply_t* reply) {
    for (int i = 0; i < WAIT_RETRIES; i++) {
        uint32_t size = message_recv(TOPIC_SERVICE_OP_REPLY, reply, sizeof(*reply));
        if (size > 0) return 1;
        set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*) WAIT_TIME_MS);
        yield();
    }
    return 0;
}

static void cmd_list(void) {
    int dummy = 0;
    message_send(TOPIC_SERVICE_LIST, &dummy, sizeof(dummy));

    service_list_reply_t reply;
    memset(&reply, 0, sizeof(reply));

    if (!wait_for_list_reply(&reply)) {
        printf("service: No reply from scheduler\n");
        return;
    }

    if (reply.count == 0) {
        printf("No services registered\n");
        return;
    }

    printf("%-32s %-10s %-8s %s\n", "NAME", "STATUS", "PID", "RETRIES");
    printf("%-32s %-10s %-8s %s\n", "----", "------", "---", "-------");
    for (int i = 0; i < reply.count; i++) {
        service_info_t* svc = &reply.services[i];
        const char* status_str;
        switch (svc->status) {
            case SERVICE_STATUS_RUNNING: status_str = "running"; break;
            case SERVICE_STATUS_STOPPED: status_str = "stopped"; break;
            case SERVICE_STATUS_FAILED:  status_str = "failed";  break;
            default:                     status_str = "unknown"; break;
        }
        printf("%-32s %-10s %-8d %d\n", svc->name, status_str, svc->pid, svc->retry);
    }
}

static int cmd_op(uint32_t topic, const char* name, const char* op_name) {
    service_op_request_t req;
    memset(&req, 0, sizeof(req));
    size_t slen = strnlen(name, SERVICE_NAME_LEN - 1);
    memcpy(req.name, name, slen);
    req.name[slen] = 0;

    message_send(topic, &req, sizeof(req));

    service_op_reply_t reply;
    memset(&reply, 0, sizeof(reply));

    if (!wait_for_op_reply(&reply)) {
        printf("service: No reply from scheduler\n");
        return 1;
    }

    if (reply.success) {
        printf("service: %s '%s': %s\n", op_name, name, reply.message);
        return 0;
    } else {
        printf("service: Failed to %s '%s': %s\n", op_name, name, reply.message);
        return 1;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: service <list|start|stop|restart> [name]\n");
        return 1;
    }

    if (strcmp(argv[1], "list") == 0) {
        cmd_list();
        return 0;
    } else if (strcmp(argv[1], "start") == 0) {
        if (argc < 3) {
            printf("Usage: service start <name>\n");
            return 1;
        }
        return cmd_op(TOPIC_SERVICE_START, argv[2], "start");
    } else if (strcmp(argv[1], "stop") == 0) {
        if (argc < 3) {
            printf("Usage: service stop <name>\n");
            return 1;
        }
        return cmd_op(TOPIC_SERVICE_STOP, argv[2], "stop");
    } else if (strcmp(argv[1], "restart") == 0) {
        if (argc < 3) {
            printf("Usage: service restart <name>\n");
            return 1;
        }
        return cmd_op(TOPIC_SERVICE_RESTART, argv[2], "restart");
    } else {
        printf("service: Unknown subcommand '%s'\n", argv[1]);
        printf("Usage: service <list|start|stop|restart> [name]\n");
        return 1;
    }
}
