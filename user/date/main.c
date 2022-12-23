#include <sys/time.h>
#include <buildin/unix_time.h>
#include <stdio.h>

int main() {
	long unix_time = time();

	char date[128] = { 0 };
	unix_time_to_string(unix_time, date);

	printf("%s\n", date);

	return 0;
}