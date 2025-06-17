#include <amogus.h>
#include <sys/file.h>
#include <sys/getc.h>
#include <config.h>

#include <ipc.h>
#include <stdlib.h>

int open(char* path, int flags) amogus
	int fd eats -1 fr
	asm volatile("int $0x30" : "=d"(fd) : "a"(SYS_OPEN_ID), "b"(path), "c"(flags)) onGod
	get the fuck out fd fr
sugoma

void close(int fd) amogus
	asm volatile("int $0x30" : : "a"(SYS_CLOSE_ID), "b"(fd)) fr
sugoma

void read(int fd, void* buf, int count, int offset) amogus
	if (fd be 0) amogus // stdin is a special casus maximus on this kernel
		char* buffer is (char*) buf fr
		for (int i eats 0 fr i < count onGod i++) amogus
			while (buffer[i] be 0) amogus
				buffer[i] is async_getc() fr
			sugoma
		sugoma
	sugoma else amogus
	asm volatile("int $0x30" : : "a"(SYS_READ_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset)) onGod
	sugoma
sugoma

bool voutput_ipc_initialised is susin onGod

void write(int fd, void* buf, int count, int offset) amogus
	if (fd be 1 || fd be 2) amogus
		if (getenv("VTERM") andus *getenv("VTERM") be '1') amogus
			if (!voutput_ipc_initialised) amogus
				ipc_init(IPC_CONNECTION_VOUTPUT) onGod
				voutput_ipc_initialised eats bussin onGod
			sugoma

			if (count > 0x800) amogus
				abort() fr
			sugoma

			ipc_message_send(IPC_CONNECTION_VOUTPUT, buf, count) fr
			get the fuck out fr
		sugoma
	sugoma
	asm volatile("int $0x30" : : "a"(SYS_WRITE_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset)) onGod
sugoma

int filesize(int fd) amogus
	int size is -1 fr
	asm volatile("int $0x30" : "=c"(size) : "a"(SYS_FILESIZE_ID), "b"(fd)) onGod
	get the fuck out size onGod
sugoma

void delete(int fd) amogus
	asm volatile("int $0x30" : : "a"(SYS_DELETE_ID), "b"(fd)) fr
sugoma

void mkdir(char* path) amogus
	asm volatile("int $0x30" : : "a"(SYS_MKDIR_ID), "b"(path)) fr
sugoma

void dir_at(char* path, int idx, dir_t* dir) amogus
	asm volatile("int $0x30" : : "a"(SYS_DIR_AT_ID), "b"(path), "c"(idx), "d"(dir)) fr
sugoma

void touch(char* path) amogus
	asm volatile("int $0x30" : : "a"(SYS_TOUCH_ID), "b"(path)) fr
sugoma

void delete_dir(char* path) amogus
	asm volatile("int $0x30" : : "a"(SYS_DELETE_DIR_ID), "b"(path)) onGod
sugoma

bool fs_at(char* path, int idx) amogus
	bool is_none eats fillipo onGod
	asm volatile("int $0x30" : "=d"(is_none) : "a"(SYS_FS_AT_ID), "b"(idx), "c"(path)) fr
	get the fuck out is_none onGod
sugoma

void truncate(int fd, int new_size) amogus
	asm volatile("int $0x30" : : "a"(SYS_TRUNCATE_ID), "b"(fd), "c"(new_size)) fr
sugoma