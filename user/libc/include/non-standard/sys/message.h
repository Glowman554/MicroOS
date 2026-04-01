#pragma once

#include <stdint.h>

#define TOPIC_RESOLVED 1
#define TOPIC_RESOLVED_REPLY 2
#define TOPIC_SHORTCUT_LAUNCH 3
#define TOPIC_SERVICE_LIST       4
#define TOPIC_SERVICE_LIST_REPLY 5
#define TOPIC_SERVICE_START      6
#define TOPIC_SERVICE_STOP       7
#define TOPIC_SERVICE_RESTART    8
#define TOPIC_SERVICE_OP_REPLY   9

#define SERVICE_STATUS_RUNNING  0
#define SERVICE_STATUS_STOPPED  1
#define SERVICE_STATUS_FAILED   2

#define SERVICE_NAME_LEN 64
#define MAX_SERVICES     32
#define MAX_SERVICE_CMD_LEN 64

typedef struct {
    char name[SERVICE_NAME_LEN];
    char command[MAX_SERVICE_CMD_LEN];
    int  status;
    int  pid;
    int  retry;
} service_info_t;

typedef struct {
    int count;
    service_info_t services[MAX_SERVICES];
} service_list_reply_t;

typedef struct {
    char name[SERVICE_NAME_LEN];
} service_op_request_t;

typedef struct {
    int  success;
    char message[64];
} service_op_reply_t;

void message_send(uint32_t topic, void* message, uint32_t size);
uint32_t message_recv(uint32_t topic, void* buffer, uint32_t size);
