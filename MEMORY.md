# Memory

- 0xB0000000: Programs should be linked here (if they aren't it will still work as long as they don't overlap with other memory)
- 0xC0000000: Starting from here will be the stack for the programs (Stack doesen't exactly start at 0xC0000000, the memory address will vary. The kernel will set up the stack for the programs). argv and envp will be stored here too.
- 0xD0000000: The heap will start here
- 0xF0000000: Virtual output ipc connection
- 0xF0001000: Terminal ipc connection
