import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function stdio() {
    functionGen(id(), "int gets(char* buffer);");
    functionGen(id(), "char getchar();");
    functionGen(id(), "void putchar(char c);");
}



