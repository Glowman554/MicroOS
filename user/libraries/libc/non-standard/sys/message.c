#include <non-standard/sys/message.h>
#include <config.h>

void message_send(uint32_t topic, void* message, uint32_t size) {
	asm volatile("int $0x30" : : "a"(SYS_MESSAGE_SEND_ID), "b"(topic), "c"(message), "d"(size));
}

uint32_t message_recv(uint32_t topic, void* buffer, uint32_t size) {
	uint32_t received;
	asm volatile("int $0x30" : "=a"(received) : "a"(SYS_MESSAGE_RECV_ID), "b"(topic), "c"(buffer), "d"(size));
	return received;
}
