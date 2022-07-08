# Memory

- The first 32 megabyte of ram are identity mapped for the kernel. The kernel code lives in the first 32 megabytes of ram.
- 0xA0000000: Programs should be linked here (if they aren't it will still work as long as they don't overlap with other memory)
- 0xC0000000: Starting from here will be the stack for the programs (Stack doesen't exactly start at 0xC0000000, the memory address will vary. The kernel will set up the stack for the programs). argv and envp will be stored here too.
- 0xD0000000: The heap will start here