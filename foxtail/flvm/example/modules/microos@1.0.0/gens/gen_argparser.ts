import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function argparser() {
    functionGen(id(), "bool is_arg(char* in, char* arg, char* out);");
}