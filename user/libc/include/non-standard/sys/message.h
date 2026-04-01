#pragma once

#include <stdint.h>

void message_send(uint32_t topic, void* message, uint32_t size);
uint32_t message_recv(uint32_t topic, void* buffer, uint32_t size);
