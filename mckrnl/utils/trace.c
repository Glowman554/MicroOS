#include <utils/trace.h>


void stack_unwind(int max, void (*callback)(int frame_num, uint32_t eip)) {
	stackframe_t* frame;
	asm volatile ("movl %%ebp, %0" : "=r"(frame));

	for(int f = 0; frame && f < max; f++) {
		callback(f, frame->eip);
		frame = frame->ebp;
	}
}