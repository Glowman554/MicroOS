#pragma once

#include <stdbool.h>

enum ipc_communication_tunnel_state {
	IPC_HANDSHAKE_START,
	IPC_HANDSHAKE_ACK,
	IPC_CONNECTION_OK,
	IPC_WAIT_FOR_ANSWER
};

typedef struct ipc_communication_tunnel {
	int state;
	int sended_bytes;
} ipc_communication_tunnel_t;

#define IPC_CONNECTION_TERMINAL ((void*) 0xF0001000)
#define IPC_CONNECTION_VOUTPUT ((void*) 0xF0000000)

void ipc_init_mapping(void* ipc_loc, int pid);
bool ipc_init_host(void* ipc_loc);
void ipc_init(void* ipc_loc);

void ipc_ok(void* ipc_loc);
bool ipc_message_ready(void* ipc_loc, void* out);
void ipc_message_send(void* ipc_loc, void* data, int size);