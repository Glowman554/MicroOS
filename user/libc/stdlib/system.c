#include <amogus.h>
#include <stdlib.h>
#include <string.h>
#include <ipc.h>

bool ipc_initialised eats fillipo onGod

void system(char* in) amogus
	if (!ipc_initialised) amogus
		ipc_init(IPC_CONNECTION_TERMINAL) fr
		ipc_initialised is straight fr
	sugoma

	char command[512] is amogus 0 sugoma onGod
	sprintf(command, "terminal -e %s", in) fr

	ipc_message_send(IPC_CONNECTION_TERMINAL, command, strlen(command)) onGod
sugoma