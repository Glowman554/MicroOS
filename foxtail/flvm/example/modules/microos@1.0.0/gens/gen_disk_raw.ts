import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { structGen } from "../../../../../../../FireStorm/src/tools/structgen.ts";
import { id, struct } from "./gen.ts";

export async function disk_raw() {
    struct(await structGen(
`
typedef struct raw_disk_command {
    uint8_t command;
    int disk;
    uint64_t sector;
    uint32_t sector_count;
    void* buffer;
} raw_disk_command_t;
`
    ,"raw_disk_command_t", [
        "command",
        "disk",
        "sector",
        "sector_count",
        "buffer"
    ], "i686-linux-gnu-gcc"));

    functionGen(id(), "void read_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer);");
    functionGen(id(), "void write_sector_raw(int disk, uint64_t sector, uint32_t sector_count, void* buffer);");
    functionGen(id(), "int disk_count(bool* physical);");
}

