import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function mmap() {
    functionGen(id(), "void mmap(void* addr);");
    functionGen(id(), "void mmmap(void* addr, void* addr_rem, int pid);");
}



