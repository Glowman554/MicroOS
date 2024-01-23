import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function graphics() {
    functionGen(id(), "int get_width();");
    functionGen(id(), "int get_height();");
    functionGen(id(), "void draw_char(int x, int y, char c, int color);");
    functionGen(id(), "void draw_string(int x, int y, char* str, int color);");
    functionGen(id(), "void set_pixel(int x, int y, int color);");
    functionGen(id(), "void start_frame();");
    functionGen(id(), "void end_frame();");
}

