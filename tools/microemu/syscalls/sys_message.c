#include <emu.h>
#include <stdlib.h>
#include <string.h>

void sys_set_term(uint32_t ebx, uint32_t ecx) {
}

typedef struct msg_node {
    void* data;
    size_t size;
    struct msg_node* next;
} msg_node_t;

typedef struct msg_topic {
    uint32_t id;
    msg_node_t* head;
    msg_node_t* tail;
    struct msg_topic* next;
} msg_topic_t;

static msg_topic_t* topics = NULL;

static msg_topic_t* get_or_create_topic(uint32_t id) {
    for (msg_topic_t *t = topics; t; t = t->next) {
        if (t->id == id) {
            return t;
        }
    }

    msg_topic_t* t = calloc(1, sizeof(*t));
    t->id = id;
    t->next = topics;
    topics = t;
    return t;
}

void sys_message_send(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    uint32_t topic_id = ebx;
    uint32_t buf_addr = ecx;
    uint32_t size = edx;

    if (size == 0) {
        return;
    }

    void* buf = malloc(size);
    if (uc_mem_read(uc, buf_addr, buf, size) != UC_ERR_OK) {
        free(buf);
        return;
    }

    msg_topic_t* topic = get_or_create_topic(topic_id);
    msg_node_t* node  = calloc(1, sizeof(*node));
    node->data = buf;
    node->size = size;

    if (topic->tail) {
        topic->tail->next = node;
        topic->tail = node;
    } else {
        topic->head = topic->tail = node;
    }
}

void sys_message_recv(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    uint32_t topic_id = ebx;
    uint32_t buf_addr = ecx;
    uint32_t buf_size = edx;

    msg_topic_t* topic = get_or_create_topic(topic_id);
    msg_node_t* node  = topic->head;

    if (!node) {
        uint32_t zero = 0;
        uc_reg_write(uc, UC_X86_REG_EAX, &zero);
        return;
    }

    topic->head = node->next;
    if (!topic->head) {
        topic->tail = NULL;
    }

    uint32_t copy = node->size < buf_size ? (uint32_t)node->size : buf_size;
    uc_mem_write(uc, buf_addr, node->data, copy);
    uc_reg_write(uc, UC_X86_REG_EAX, &copy);

    free(node->data);
    free(node);
}
