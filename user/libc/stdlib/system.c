#include <stdlib.h>
#include <string.h>
#include <non-standart/buildin/ipc.h>

bool ipc_initialised = false;

int system(const char* in) {
	if (!ipc_initialised) {
		ipc_init(IPC_CONNECTION_TERMINAL);
		ipc_initialised = true;
	}

	char command[512] = { 0 };
	sprintf(command, "terminal -e %s", in);

	ipc_message_send(IPC_CONNECTION_TERMINAL, command, strlen(command));
	return 0;
}