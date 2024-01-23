import { functionGen } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function ipc() {
    functionGen(id(), "void ipc_init_mapping(void* ipc_loc, int pid);");
    functionGen(id(), "bool ipc_init_host(void* ipc_loc);");
    functionGen(id(), "void ipc_init(void* ipc_loc);");
    functionGen(id(), "void ipc_ok(void* ipc_loc);");
    functionGen(id(), "bool ipc_message_ready(void* ipc_loc, void* out);");
    functionGen(id(), "void ipc_message_send(void* ipc_loc, void* data, int size);");
}
