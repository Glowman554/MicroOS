import { functionGen, addDatatype } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { structGen } from "../../../../../../../FireStorm/src/tools/structgen.ts";
import { id, struct } from "./gen.ts";

export async function spawn() {
    struct(await structGen(
`
typedef struct task_list {
	char name[128];
	int pid;
} task_list_t;
`,
    "task_list_t", [
        "name",
        "pid"
    ], "i686-linux-gnu-gcc"));


    addDatatype("task_list_t*", "int");

    functionGen(id(), "int spawn(const char* path, const char** argv, const char** envp);");
    functionGen(id(), "bool get_proc_info(int pid);");
    functionGen(id(), "void yield();");
    functionGen(id(), "int get_task_list(task_list_t* out, int max);");
    functionGen(id(), "void kill(int pid);");
}
