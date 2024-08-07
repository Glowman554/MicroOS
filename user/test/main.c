// #include <stdio.h>
// #include <buildin/time.h>
// #include <sys/spawn.h>

// void test_thread() {
// 	FILE* file = fopen("README.md", "r");
// 	FILE* file2 = fopen("README.md", "r");
// 	fsize(file, test);

// 	fclose(file2);

// 	while (true) {
// 		printf("Hello thread %d\n", test);
// 		sleep_s(1);
// 	}
// }

// int main(int argc, char* argv[], char* envp[]) {
// 	int child = thread(test_thread);

// 	sleep_ms(500);

// 	for (int i = 0; i < 60; i++) {
// 		printf("Hello %d\n", i);
// 		sleep_s(1);

// 		if (i == 30) {
// 			kill(child);
// 		}
// 	}

// 	return 0;
// }

#include <buildin/graphics.h>
#include <buildin/mouse.h>
#include <sys/getc.h>

int main(int argc, char* argv[], char* envp[]) {
	while (async_getc() != 27) {
		start_frame();
		draw_string(0, 0, "Hello world", 1);
		update_mouse();
		end_frame();
	}
	return 0;
}