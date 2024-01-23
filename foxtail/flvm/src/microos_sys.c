
#include <stdint.h>
#include <vm.h>
#include <sys/env.h>
#include <sys/getc.h>
#include <sys/mmap.h>
#include <sys/mouse.h>
#include <sys/net.h>
#include <sys/spawn.h>
void native_env(struct vm_instance* vm) {
    int id = (int) stack_pop(vm);
    stack_push(vm, env(id));
}
void native_set_env(struct vm_instance* vm) {
    void* value = (void*) stack_pop(vm);
    int id = (int) stack_pop(vm);
    set_env(id, value);
    stack_push(vm, 0);
}
void native_async_getc(struct vm_instance* vm) {
    stack_push(vm, async_getc());
}
void native_async_getarrw(struct vm_instance* vm) {
    stack_push(vm, async_getarrw());
}
void native_mmap(struct vm_instance* vm) {
    void* addr = (void*) stack_pop(vm);
    mmap(addr);
    stack_push(vm, 0);
}
void native_mmmap(struct vm_instance* vm) {
    int pid = (int) stack_pop(vm);
    void* addr_rem = (void*) stack_pop(vm);
    void* addr = (void*) stack_pop(vm);
    mmmap(addr, addr_rem, pid);
    stack_push(vm, 0);
}
void native_mouse_info(struct vm_instance* vm) {
    mouse_info_t* info = (mouse_info_t*) stack_pop(vm);
    mouse_info(info);
    stack_push(vm, 0);
}
void native_icmp_ping(struct vm_instance* vm) {
    ip_u ip = (ip_u) (uint32_t) stack_pop(vm);
    int nic = (int) stack_pop(vm);
    stack_push(vm, icmp_ping(nic, ip));
}
void native_dns_resolve_A(struct vm_instance* vm) {
    const char* domain = (const char*) stack_pop(vm);
    int nic = (int) stack_pop(vm);
    stack_push(vm, dns_resolve_A(nic, domain).ip);
}
void native_connect(struct vm_instance* vm) {
    uint16_t port = (uint16_t) stack_pop(vm);
    ip_u ip = (ip_u) (uint32_t) stack_pop(vm);
    int type = (int) stack_pop(vm);
    int nic = (int) stack_pop(vm);
    stack_push(vm, connect(nic, type, ip, port));
}
void native_disconnect(struct vm_instance* vm) {
    int sock = (int) stack_pop(vm);
    disconnect(sock);
    stack_push(vm, 0);
}
void native_send(struct vm_instance* vm) {
    int size = (int) stack_pop(vm);
    uint8_t* data = (uint8_t*) stack_pop(vm);
    int sock = (int) stack_pop(vm);
    send(sock, data, size);
    stack_push(vm, 0);
}
void native_recv(struct vm_instance* vm) {
    int size = (int) stack_pop(vm);
    uint8_t* data = (uint8_t*) stack_pop(vm);
    int sock = (int) stack_pop(vm);
    stack_push(vm, recv(sock, data, size));
}
void native_spawn(struct vm_instance* vm) {
    const char** envp = (const char**) stack_pop(vm);
    const char** argv = (const char**) stack_pop(vm);
    const char* path = (const char*) stack_pop(vm);
    stack_push(vm, spawn(path, argv, envp));
}
void native_get_proc_info(struct vm_instance* vm) {
    int pid = (int) stack_pop(vm);
    stack_push(vm, get_proc_info(pid));
}
void native_yield(struct vm_instance* vm) {
    yield();
    stack_push(vm, 0);
}
void native_get_task_list(struct vm_instance* vm) {
    int max = (int) stack_pop(vm);
    task_list_t* out = (task_list_t*) stack_pop(vm);
    stack_push(vm, get_task_list(out, max));
}
void native_kill(struct vm_instance* vm) {
    int pid = (int) stack_pop(vm);
    kill(pid);
    stack_push(vm, 0);
}
void microos_sys() {
    vm_native_register(5328983, native_env);
    vm_native_register(5328984, native_set_env);
    vm_native_register(5328985, native_async_getc);
    vm_native_register(5328986, native_async_getarrw);
    vm_native_register(5328987, native_mmap);
    vm_native_register(5328988, native_mmmap);
    vm_native_register(5328989, native_mouse_info);
    vm_native_register(5328990, native_icmp_ping);
    vm_native_register(5328991, native_dns_resolve_A);
    vm_native_register(5328992, native_connect);
    vm_native_register(5328993, native_disconnect);
    vm_native_register(5328994, native_send);
    vm_native_register(5328995, native_recv);
    vm_native_register(5328996, native_spawn);
    vm_native_register(5328997, native_get_proc_info);
    vm_native_register(5328998, native_yield);
    vm_native_register(5328999, native_get_task_list);
    vm_native_register(5329000, native_kill);
}