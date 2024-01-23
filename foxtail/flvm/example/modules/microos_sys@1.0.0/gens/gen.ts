import { addDatatype, finish } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { env } from "./gen_env.ts";
import { getc } from "./gen_getc.ts";
import { mmap } from "./gen_mmap.ts";
import { mouse } from "./gen_mouse.ts";
import { net } from "./gen_net.ts";
import { spawn } from "./gen_spawn.ts";

let cid = 5328983;
export function id() {
    return cid++;
}

const structs: string[] = [];
export function struct(s: string) {
    structs.push(s);
}

addDatatype("int", "int");
addDatatype("uint16_t", "int");
addDatatype("uint8_t*", "chr[]");
addDatatype("void*", "int");
addDatatype("const char", "chr");
addDatatype("const char**", "str[]");

await env();
await getc();
await mmap();
await mouse();
await net();
await spawn();

const includes =
`
#include <vm.h>
#include <sys/env.h>
#include <sys/getc.h>
#include <sys/mmap.h>
#include <sys/mouse.h>
#include <sys/net.h>
#include <sys/spawn.h>
`;

const [fl, c] = finish(includes, "microos_sys");

let finalfl = "";

for (const s of structs) {
    finalfl += s;
}

finalfl += fl;

Deno.writeTextFileSync("microos_sys.fl", finalfl);
Deno.writeTextFileSync("microos_sys.c", c);