import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function env() {
    functionGen(id(), "void* env(int id);");
    functionGen(id(), "void set_env(int id, void* value);");
}
