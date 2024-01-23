import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function stdlib() {
    functionGen(id(), "void print_allocations(const char* msg);");
    functionGen(id(), "char* getenv(const char* name);");
    functionGen(id(), "void system(char* in);");
}



