
#include <vm.h>
#include <stdbool.h>

#include <buildin/time.h>
#include <buildin/disk_raw.h>
#include <buildin/graphics.h>
#include <argparser.h>
#include <stdio.h>
#include <stdlib.h>
#include <ipc.h>
void native_is_arg(struct vm_instance* vm) {
    char* out = (char*) stack_pop(vm);
    char* arg = (char*) stack_pop(vm);
    char* in = (char*) stack_pop(vm);
    stack_push(vm, is_arg(in, arg, out));
}
void native_ipc_init_mapping(struct vm_instance* vm) {
    int pid = (int) stack_pop(vm);
    void* ipc_loc = (void*) stack_pop(vm);
    ipc_init_mapping(ipc_loc, pid);
    stack_push(vm, 0);
}
void native_ipc_init_host(struct vm_instance* vm) {
    void* ipc_loc = (void*) stack_pop(vm);
    stack_push(vm, ipc_init_host(ipc_loc));
}
void native_ipc_init(struct vm_instance* vm) {
    void* ipc_loc = (void*) stack_pop(vm);
    ipc_init(ipc_loc);
    stack_push(vm, 0);
}
void native_ipc_ok(struct vm_instance* vm) {
    void* ipc_loc = (void*) stack_pop(vm);
    ipc_ok(ipc_loc);
    stack_push(vm, 0);
}
void native_ipc_message_ready(struct vm_instance* vm) {
    void* out = (void*) stack_pop(vm);
    void* ipc_loc = (void*) stack_pop(vm);
    stack_push(vm, ipc_message_ready(ipc_loc, out));
}
void native_ipc_message_send(struct vm_instance* vm) {
    int size = (int) stack_pop(vm);
    void* data = (void*) stack_pop(vm);
    void* ipc_loc = (void*) stack_pop(vm);
    ipc_message_send(ipc_loc, data, size);
    stack_push(vm, 0);
}
void native_gets(struct vm_instance* vm) {
    char* buffer = (char*) stack_pop(vm);
    stack_push(vm, gets(buffer));
}
void native_getchar(struct vm_instance* vm) {
    stack_push(vm, getchar());
}
void native_putchar(struct vm_instance* vm) {
    char c = (char) stack_pop(vm);
    putchar(c);
    stack_push(vm, 0);
}
void native_print_allocations(struct vm_instance* vm) {
    const char* msg = (const char*) stack_pop(vm);
    print_allocations(msg);
    stack_push(vm, 0);
}
void native_getenv(struct vm_instance* vm) {
    const char* name = (const char*) stack_pop(vm);
    stack_push(vm, getenv(name));
}
void native_system(struct vm_instance* vm) {
    char* in = (char*) stack_pop(vm);
    system(in);
    stack_push(vm, 0);
}
void native_read_sector_raw(struct vm_instance* vm) {
    void* buffer = (void*) stack_pop(vm);
    uint32_t sector_count = (uint32_t) stack_pop(vm);
    uint64_t sector = (uint64_t) stack_pop(vm);
    int disk = (int) stack_pop(vm);
    read_sector_raw(disk, sector, sector_count, buffer);
    stack_push(vm, 0);
}
void native_write_sector_raw(struct vm_instance* vm) {
    void* buffer = (void*) stack_pop(vm);
    uint32_t sector_count = (uint32_t) stack_pop(vm);
    uint64_t sector = (uint64_t) stack_pop(vm);
    int disk = (int) stack_pop(vm);
    write_sector_raw(disk, sector, sector_count, buffer);
    stack_push(vm, 0);
}
void native_disk_count(struct vm_instance* vm) {
    bool* physical = (bool*) stack_pop(vm);
    stack_push(vm, disk_count(physical));
}
void native_get_width(struct vm_instance* vm) {
    stack_push(vm, get_width());
}
void native_get_height(struct vm_instance* vm) {
    stack_push(vm, get_height());
}
void native_draw_char(struct vm_instance* vm) {
    int color = (int) stack_pop(vm);
    char c = (char) stack_pop(vm);
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    draw_char(x, y, c, color);
    stack_push(vm, 0);
}
void native_draw_string(struct vm_instance* vm) {
    int color = (int) stack_pop(vm);
    char* str = (char*) stack_pop(vm);
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    draw_string(x, y, str, color);
    stack_push(vm, 0);
}
void native_set_pixel(struct vm_instance* vm) {
    int color = (int) stack_pop(vm);
    int y = (int) stack_pop(vm);
    int x = (int) stack_pop(vm);
    set_pixel(x, y, color);
    stack_push(vm, 0);
}
void native_start_frame(struct vm_instance* vm) {
    start_frame();
    stack_push(vm, 0);
}
void native_end_frame(struct vm_instance* vm) {
    end_frame();
    stack_push(vm, 0);
}
void native_time_format(struct vm_instance* vm) {
    __libc_time_t* time = (__libc_time_t*) stack_pop(vm);
    char* out = (char*) stack_pop(vm);
    time_format(out, time);
    stack_push(vm, 0);
}
void native_sleep_s(struct vm_instance* vm) {
    int s = (int) stack_pop(vm);
    sleep_s(s);
    stack_push(vm, 0);
}
void native_sleep_ms(struct vm_instance* vm) {
    int ms = (int) stack_pop(vm);
    sleep_ms(ms);
    stack_push(vm, 0);
}
void microos() {
    vm_native_register(25738794, native_is_arg);
    vm_native_register(25738795, native_ipc_init_mapping);
    vm_native_register(25738796, native_ipc_init_host);
    vm_native_register(25738797, native_ipc_init);
    vm_native_register(25738798, native_ipc_ok);
    vm_native_register(25738799, native_ipc_message_ready);
    vm_native_register(25738800, native_ipc_message_send);
    vm_native_register(25738801, native_gets);
    vm_native_register(25738802, native_getchar);
    vm_native_register(25738803, native_putchar);
    vm_native_register(25738804, native_print_allocations);
    vm_native_register(25738805, native_getenv);
    vm_native_register(25738806, native_system);
    vm_native_register(25738807, native_read_sector_raw);
    vm_native_register(25738808, native_write_sector_raw);
    vm_native_register(25738809, native_disk_count);
    vm_native_register(25738810, native_get_width);
    vm_native_register(25738811, native_get_height);
    vm_native_register(25738812, native_draw_char);
    vm_native_register(25738813, native_draw_string);
    vm_native_register(25738814, native_set_pixel);
    vm_native_register(25738815, native_start_frame);
    vm_native_register(25738816, native_end_frame);
    vm_native_register(25738817, native_time_format);
    vm_native_register(25738818, native_sleep_s);
    vm_native_register(25738819, native_sleep_ms);
}