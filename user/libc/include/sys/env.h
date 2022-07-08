#pragma once

#define SYS_GET_ARGV_ID 0x00
#define SYS_GET_ENVP_ID 0x01
#define SYS_GET_PWD_ID 0x02
#define SYS_SET_PWD_ID 0x03

void* env(int id);
void set_env(int id, void* value);