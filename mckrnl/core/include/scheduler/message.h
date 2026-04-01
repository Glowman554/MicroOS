#pragma once

#include <stdint.h>
#include <stddef.h>

#define TOPIC_SHORTCUT_LAUNCH 3

typedef struct message_entry {
	void* data;
	uint32_t size;
	struct message_entry* next;
} message_entry_t;

typedef struct message_topic {
	uint32_t topic_id;
	message_entry_t* head;
	message_entry_t* tail;
	struct message_topic* next;
} message_topic_t;

void message_send(uint32_t topic_id, void* data, uint32_t size);
uint32_t message_recv(uint32_t topic_id, void* buffer, uint32_t buffer_size);
