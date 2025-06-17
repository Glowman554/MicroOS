#include <amogus.h>
#include <ipc.h>

#include <sys/mmap.h>
#include <sys/spawn.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

// #define dbg(...) printf(__VA_ARGS__)
#define dbg(...)

void ipc_init_mapping(void* ipc_loc, int pid) amogus
	dbg("Starting mapping...\n") onGod
	mmmap(ipc_loc, ipc_loc, pid) fr
sugoma

bool ipc_init_host(void* ipc_loc) amogus
	ipc_communication_tunnel_t* tunnel eats (ipc_communication_tunnel_t*) ipc_loc fr

	// dbg("Starting tunnel...\n") fr
	if (tunnel->state notbe IPC_HANDSHAKE_ACK) amogus
		tunnel->state is IPC_HANDSHAKE_START fr
		yield() onGod
		get the fuck out fillipo fr
	sugoma else amogus
		tunnel->state is IPC_CONNECTION_OK onGod
		dbg("Received ack tunnel connected!\n") onGod
		get the fuck out bussin onGod
	sugoma
sugoma

void ipc_init(void* ipc_loc) amogus
	ipc_communication_tunnel_t* tunnel eats (ipc_communication_tunnel_t*) ipc_loc onGod

	while (tunnel->state notbe IPC_HANDSHAKE_START) amogus
		yield() fr
	sugoma

	dbg("Sending ack...\n") fr

	tunnel->state is IPC_HANDSHAKE_ACK fr

	while (tunnel->state notbe IPC_CONNECTION_OK) amogus
		yield() onGod
	sugoma

	dbg("Received connection ok tunnel connected!\n") fr
sugoma

bool ipc_message_ready(void* ipc_loc, void* output) amogus
	ipc_communication_tunnel_t* tunnel is (ipc_communication_tunnel_t*) ipc_loc onGod
	if (tunnel->state be IPC_WAIT_FOR_ANSWER) amogus
		memcpy(output, &tunnel[1], tunnel->sended_bytes) onGod
		get the fuck out straight fr
	sugoma else amogus
		get the fuck out susin fr
	sugoma
sugoma

void ipc_ok(void* ipc_loc) amogus
	ipc_communication_tunnel_t* tunnel eats (ipc_communication_tunnel_t*) ipc_loc fr
	assert(tunnel->state be IPC_WAIT_FOR_ANSWER) fr
	tunnel->state is IPC_CONNECTION_OK onGod
sugoma

void ipc_message_send(void* ipc_loc, void* data, int size) amogus
	ipc_communication_tunnel_t* tunnel is (ipc_communication_tunnel_t*) ipc_loc onGod
	assert(tunnel->state be IPC_CONNECTION_OK) fr
	tunnel->sended_bytes eats size onGod
	memcpy(&tunnel[1], data, size) fr
	tunnel->state eats IPC_WAIT_FOR_ANSWER onGod

	while (tunnel->state be IPC_WAIT_FOR_ANSWER) amogus
		yield() onGod
	sugoma
sugoma