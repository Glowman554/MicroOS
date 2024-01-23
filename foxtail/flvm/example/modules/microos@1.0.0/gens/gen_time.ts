import { functionGen, addDatatype } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { structGen } from "../../../../../../../FireStorm/src/tools/structgen.ts";
import { id, struct } from "./gen.ts";

export async function time() {
    addDatatype("__libc_time_t*", "int");

    struct(await structGen(
`
typedef struct clock_result {
	int seconds;
	int minutes;
	int hours;
	int day;
	int month;
	int year;
} clock_result_t;
`
    ,"clock_result_t", [
        "seconds",
        "minutes",
        "hours",
        "day",
        "month",
        "year"
    ], "i686-linux-gnu-gcc"));

    functionGen(id(), "void time_format(char* out, __libc_time_t* time);");
    functionGen(id(), "void sleep_s(int s);");
    functionGen(id(), "void sleep_ms(int ms);");

}

