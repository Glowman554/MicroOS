interface Syscall {
    name: string;
    arguments: string[];
}

interface SyscallsList {
    syscalls: Syscall[];
}

const syscallList = JSON.parse(Deno.readTextFileSync("config/syscalls.json")) as SyscallsList;

let syscallListMd = "# Syscalls\n\n| Syscall | eax | ebx | ecx | edx | esi | edi |\n|---------|-----|-----|-----|-----|-----|-----|\n";

for (const syscall of syscallList.syscalls) {
    const finalArguments = [...syscall.arguments];
    for (let i = finalArguments.length; i < 6; i++) {
        finalArguments.push("-");
    }

    syscallListMd += `| ${syscall.name} | ${finalArguments.join(" | ")} |\n`;
}

Deno.writeTextFileSync("SYSCALLS.md", syscallListMd);