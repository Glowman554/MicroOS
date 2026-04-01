#include <scheduler/message.h>
#include <memory/heap.h>
#include <string.h>
#include <stddef.h>

static message_topic_t* topics = NULL;

static message_topic_t* find_topic(uint32_t topic_id) {
	message_topic_t* topic = topics;
	while (topic != NULL) {
		if (topic->topic_id == topic_id) {
			return topic;
		}
		topic = topic->next;
	}
	return NULL;
}

static message_topic_t* find_or_create_topic(uint32_t topic_id) {
	message_topic_t* topic = find_topic(topic_id);
	if (topic != NULL) {
		return topic;
	}

	topic = kmalloc(sizeof(message_topic_t));
	topic->topic_id = topic_id;
	topic->head = NULL;
	topic->tail = NULL;
	topic->next = topics;
	topics = topic;
	return topic;
}

void message_send(uint32_t topic_id, void* data, uint32_t size) {
	message_topic_t* topic = find_or_create_topic(topic_id);

	message_entry_t* entry = kmalloc(sizeof(message_entry_t));
	entry->data = kmalloc(size);
	entry->size = size;
	entry->next = NULL;

	memcpy(entry->data, data, size);

	if (topic->tail != NULL) {
		topic->tail->next = entry;
	} else {
		topic->head = entry;
	}
	topic->tail = entry;
}

uint32_t message_recv(uint32_t topic_id, void* buffer, uint32_t buffer_size) {
	message_topic_t* topic = find_topic(topic_id);
	if (topic == NULL || topic->head == NULL) {
		return 0;
	}

	message_entry_t* entry = topic->head;
	uint32_t copy_size = entry->size < buffer_size ? entry->size : buffer_size;

	memcpy(buffer, entry->data, copy_size);

	topic->head = entry->next;
	if (topic->head == NULL) {
		topic->tail = NULL;
	}

	kfree(entry->data);
	kfree(entry);

	return copy_size;
}
