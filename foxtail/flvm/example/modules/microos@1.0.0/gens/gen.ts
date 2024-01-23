import { addDatatype, finish } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { argparser } from "./gen_argparser.ts";
import { ipc } from "./gen_ipc.ts";
import { stdio } from "./gen_stdio.ts";
import { stdlib } from "./gen_stdlib.ts";
import { disk_raw } from "./gen_disk_raw.ts";
import { graphics } from "./gen_graphics.ts";
import { time } from "./gen_time.ts";

let cid = 25738794;
export function id() {
    return cid++;
}

const structs: string[] = [];
export function struct(s: string) {
    structs.push(s);
}

addDatatype("char*", "str");
addDatatype("void*", "int");
addDatatype("int", "int");
addDatatype("uint64_t", "int");
addDatatype("bool*", "chr[]");

await argparser();
await ipc();
await stdio();
await stdlib();
await disk_raw();
await graphics();
await time();

const includes =
`
#include <vm.h>
#include <stdbool.h>

#include <buildin/time.h>
#include <buildin/disk_raw.h>
#include <buildin/graphics.h>
#include <argparser.h>
#include <stdio.h>
#include <stdlib.h>
#include <ipc.h>
`;

const [fl, c] = finish(includes, "microos");

let finalfl = "";

for (const s of structs) {
    finalfl += s;
}

finalfl += fl;

Deno.writeTextFileSync("microos.fl", finalfl);
Deno.writeTextFileSync("microos.c", c);