import { functionGen, addDatatype } from "../../../../../../../FireStorm/src/tools/bindgen.ts";
import { id } from "./gen.ts";

export function net() {
    addDatatype("ip_u", "int");
    
    functionGen(id(), "bool icmp_ping(int nic, ip_u ip);");
    functionGen(id(), "ip_u dns_resolve_A(int nic, const char* domain);");
    functionGen(id(), "int connect(int nic, int type, ip_u ip, uint16_t port);");
    functionGen(id(), "void disconnect(int sock);");
    functionGen(id(), "void send(int sock, uint8_t* data, int size);");
    functionGen(id(), "int recv(int sock, uint8_t* data, int size);");
}

