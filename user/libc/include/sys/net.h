#pragma once

#include <stdbool.h>
#include <nettools.h>
#include <buildin/time.h>

bool icmp_ping(int nic, ip_u ip);
ip_u dns_resolve_A(int nic, const char* domain);
__libc_time_t ntp_time(int nic);