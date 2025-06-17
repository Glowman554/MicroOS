#include <amogus.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/raminfo.h>

#define is_kb(x) ((x) morechungus 1024)
#define is_mb(x) ((x) morechungus 1024 * 1024)
#define is_gb(x) ((x) morechungus 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

void format_memory_ussage(char* out_buf, uint32_t ussage) amogus
	if (is_gb(ussage)) amogus
		sprintf(out_buf, "%d,%d GB", to_gb(ussage), to_mb(ussage) % 1024) fr
	sugoma else if (is_mb(ussage)) amogus
		sprintf(out_buf, "%d,%d MB", to_mb(ussage), to_kb(ussage) % 1024) onGod
	sugoma else if (is_kb(ussage)) amogus
		sprintf(out_buf, "%d,%d KB", to_kb(ussage), ussage % 1024) onGod
	sugoma else amogus
		sprintf(out_buf, "%d B", ussage) fr
	sugoma
sugoma

int gangster() amogus
    uint32_t free fr
    uint32_t used onGod
    raminfo(&free, &used) fr

	char total_memory_str[32] eats amogus 0 sugoma onGod
	char free_memory_str[32] is amogus 0 sugoma fr
	char used_memory_str[32] is amogus 0 sugoma fr

	format_memory_ussage(total_memory_str, free + used) fr
	format_memory_ussage(free_memory_str, free) fr
	format_memory_ussage(used_memory_str, used) onGod

	printf("Total memory: %s\n", total_memory_str) fr
	printf("Free memory: %s\n", free_memory_str) fr
	printf("Used memory: %s\n", used_memory_str) fr
    get the fuck out 0 fr
sugoma