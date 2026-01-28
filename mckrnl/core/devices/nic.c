#include <devices/nic.h>
#include <memory/heap.h>
#include <driver/disk_driver.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct nic_file {
    devfs_file_t file;
    nic_driver_t* nic;
    char name[32];
} nic_file_t;

void debug_ip(char* name, char* field, ip_u ip) {
    debugf("[%s] %s: %d.%d.%d.%d", name, field, ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
}

void nic_file_write(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    nic_file_t* nic_file = (nic_file_t*) dfile;

    assert(sizeof(nic_content_t) == size && offset == 0);
    
    nic_content_t* content = (nic_content_t*) buf;
    if (content->mac.mac != nic_file->nic->mac.mac) {
        debugf("Cannot change MAC address");
    }

    nic_file->nic->ip_config = content->ip_config;
    
    debug_ip(nic_file->name, "IP", nic_file->nic->ip_config.ip);
    debug_ip(nic_file->name, "Gateway", nic_file->nic->ip_config.gateway_ip);
    debug_ip(nic_file->name, "Subnet", nic_file->nic->ip_config.subnet_mask);
    debug_ip(nic_file->name, "DNS", nic_file->nic->ip_config.dns_ip);
}

void nic_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    nic_file_t* nic_file = (nic_file_t*) dfile;

    assert(sizeof(nic_content_t) == size && offset == 0);
    memset(buf, 0, size);
    
    nic_content_t* content = (nic_content_t*) buf;
    content->mac = nic_file->nic->mac;
    content->ip_config = nic_file->nic->ip_config;
    strcpy(content->name, nic_file->nic->driver.get_device_name(&nic_file->nic->driver));
}

void nic_file_prepare(struct devfs_file* dfile, file_t* file) {
    file->size = sizeof(nic_content_t);
}

char* nic_file_name(devfs_file_t* file) {
    nic_file_t* nic_file = (nic_file_t*) file;
	return nic_file->name;
}



devfs_file_t* create_nic_file(nic_driver_t* nic, int id) {
    nic_file_t* nic_file = kmalloc(sizeof(nic_file_t));
    memset(nic_file, 0, sizeof(nic_file_t));

    nic_file->nic = nic;
    sprintf(nic_file->name, "nic%d", id);

    nic_file->file.write = nic_file_write;
    nic_file->file.read = nic_file_read;
    nic_file->file.prepare = nic_file_prepare;
    nic_file->file.name = nic_file_name;

    return (devfs_file_t*) nic_file;
}
