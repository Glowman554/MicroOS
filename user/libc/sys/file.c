#include <sys/file.h>
#include <sys/getc.h>
#include <config.h>

#include <ipc.h>
#include <stdlib.h>

int open(char* path, int flags) {
	int fd = -1;
	asm volatile("int $0x30" : "=d"(fd) : "a"(SYS_OPEN_ID), "b"(path), "c"(flags));
	return fd;
}

void close(int fd) {
	asm volatile("int $0x30" : : "a"(SYS_CLOSE_ID), "b"(fd));
}

void read(int fd, void* buf, int count, int offset) {
	if (fd == 0) { // stdin is a special case on this kernel
		char* buffer = (char*) buf;
		for (int i = 0; i < count; i++) {
			while (buffer[i] == 0) {
				buffer[i] = async_getc();
			}
		}
	} else {
	asm volatile("int $0x30" : : "a"(SYS_READ_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset));
	}
}

bool voutput_ipc_initialised = false;

void write(int fd, void* buf, int count, int offset) {
	if (fd == 1 || fd == 2) {
		if (getenv("VTERM") && *getenv("VTERM") == '1') {
			if (!voutput_ipc_initialised) {
				ipc_init(IPC_CONNECTION_VOUTPUT);
				voutput_ipc_initialised = true;
			}

			if (count > 0x800) {
				abort();
			}

			ipc_message_send(IPC_CONNECTION_VOUTPUT, buf, count);
			return;
		}
	}
	asm volatile("int $0x30" : : "a"(SYS_WRITE_ID), "b"(fd), "c"(buf), "d"(count), "S"(offset));
}

int filesize(int fd) {
	int size = -1;
	asm volatile("int $0x30" : "=c"(size) : "a"(SYS_FILESIZE_ID), "b"(fd));
	return size;
}

void delete(int fd) {
	asm volatile("int $0x30" : : "a"(SYS_DELETE_ID), "b"(fd));
}

void mkdir(char* path) {
	asm volatile("int $0x30" : : "a"(SYS_MKDIR_ID), "b"(path));
}

void dir_at(char* path, int idx, dir_t* dir) {
	asm volatile("int $0x30" : : "a"(SYS_DIR_AT_ID), "b"(path), "c"(idx), "d"(dir));
}

void touch(char* path) {
	asm volatile("int $0x30" : : "a"(SYS_TOUCH_ID), "b"(path));
}

void delete_dir(char* path) {
	asm volatile("int $0x30" : : "a"(SYS_DELETE_DIR_ID), "b"(path));
}

bool fs_at(char* path, int idx) {
	bool is_none = false;
	asm volatile("int $0x30" : "=d"(is_none) : "a"(SYS_FS_AT_ID), "b"(idx), "c"(path));
	return is_none;
}

void truncate(int fd, int new_size) {
	asm volatile("int $0x30" : : "a"(SYS_TRUNCATE_ID), "b"(fd), "c"(new_size));
}