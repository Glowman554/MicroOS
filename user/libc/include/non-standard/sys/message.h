#pragma once

#include <stdint.h>

#define TOPIC_RESOLVED 1
#define TOPIC_RESOLVED_REPLY 2
#define TOPIC_SHORTCUT_LAUNCH 3

void message_send(uint32_t topic, void* message, uint32_t size);
uint32_t message_recv(uint32_t topic, void* buffer, uint32_t size);
