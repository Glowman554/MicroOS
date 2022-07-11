#include <ipc.h>

#include <sys/mmap.h>
#include <sys/spawn.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

// #define dbg(...) printf(__VA_ARGS__)
#define dbg(...)

void ipc_init_mapping(void* ipc_loc, int pid) {
	dbg("Starting mapping...\n");
	mmmap(ipc_loc, pid);
}

bool ipc_init_host(void* ipc_loc) {
	ipc_communication_tunnel_t* tunnel = (ipc_communication_tunnel_t*) ipc_loc;

	// dbg("Starting tunnel...\n");
	if (tunnel->state != IPC_HANDSHAKE_ACK) {
		tunnel->state = IPC_HANDSHAKE_START;
		yield();
		return false;
	} else {
		tunnel->state = IPC_CONNECTION_OK;
		dbg("Received ack tunnel connected!\n");
		return true;
	}
}

void ipc_init(void* ipc_loc) {
	ipc_communication_tunnel_t* tunnel = (ipc_communication_tunnel_t*) ipc_loc;

	while (tunnel->state != IPC_HANDSHAKE_START) {
		yield();
	}

	dbg("Sending ack...\n");

	tunnel->state = IPC_HANDSHAKE_ACK;

	while (tunnel->state != IPC_CONNECTION_OK) {
		yield();
	}

	dbg("Received connection ok tunnel connected!\n");
}

bool ipc_message_ready(void* ipc_loc, void* out) {
	ipc_communication_tunnel_t* tunnel = (ipc_communication_tunnel_t*) ipc_loc;
	if (tunnel->state == IPC_WAIT_FOR_ANSWER) {
		memcpy(out, &tunnel[1], tunnel->sended_bytes);
		return true;
	} else {
		return false;
	}
}

void ipc_ok(void* ipc_loc) {
	ipc_communication_tunnel_t* tunnel = (ipc_communication_tunnel_t*) ipc_loc;
	assert(tunnel->state == IPC_WAIT_FOR_ANSWER);
	tunnel->state = IPC_CONNECTION_OK;
}

void ipc_message_send(void* ipc_loc, void* data, int size) {
	ipc_communication_tunnel_t* tunnel = (ipc_communication_tunnel_t*) ipc_loc;
	assert(tunnel->state == IPC_CONNECTION_OK);
	tunnel->sended_bytes = size;
	memcpy(&tunnel[1], data, size);
	tunnel->state = IPC_WAIT_FOR_ANSWER;

	while (tunnel->state == IPC_WAIT_FOR_ANSWER) {
		yield();
	}
}