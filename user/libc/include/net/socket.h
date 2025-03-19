#pragma once

#include <sys/net.h>

int sync_connect(int nic, int type, ip_u ip, uint16_t port);