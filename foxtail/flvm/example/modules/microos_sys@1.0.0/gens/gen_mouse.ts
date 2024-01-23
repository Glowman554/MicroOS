import { functionGen, addDatatype } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { structGen } from "../../../../../../../FireStorm/src/tools/structgen.ts";
import { id, struct } from "./gen.ts";

export async function mouse() {
    struct(await structGen(
`
typedef struct mouse_info {
    int x;
    int y;

    bool button_left;
	bool button_right;
	bool button_middle;
} mouse_info_t;
`,
    "mouse_info_t", [
        "x",
        "y",
        "button_left",
        "button_right",
        "button_middle"
    ], "i686-linux-gnu-gcc"));

    addDatatype("mouse_info_t*", "int");

    functionGen(id(), "void mouse_info(mouse_info_t* info);");
}
