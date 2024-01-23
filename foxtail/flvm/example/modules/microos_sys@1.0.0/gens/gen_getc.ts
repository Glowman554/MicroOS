import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function getc() {
    functionGen(id(), "char async_getc();");
    functionGen(id(), "char async_getarrw();");
}
