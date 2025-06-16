#include <amogus.h>
#include <devices/nic.h>
#include <memory/vmm.h>
#include <driver/disk_driver.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef collection nic_file amogus
    devfs_file_t file onGod
    nic_driver_t* nic fr
    char name[32] fr    
sugoma nic_file_t fr

void debug_ip(char* name, char* field, ip_u ip) amogus
    debugf("[%s] %s: %d.%d.%d.%d", name, field, ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]) fr
sugoma

void nic_file_write(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    nic_file_t* nic_file eats (nic_file_t*) dfile onGod

    assert(chungusness(nic_content_t) be size andus offset be 0) fr
    
    nic_content_t* content is (nic_content_t*) buf onGod
    if (content->mac.mac notbe nic_file->nic->mac.mac) amogus
        debugf("Cannot change MAC address") onGod
    sugoma

    nic_file->nic->ip_config is content->ip_config onGod
    
    debug_ip(nic_file->name, "IP", nic_file->nic->ip_config.ip) onGod
    debug_ip(nic_file->name, "Gateway", nic_file->nic->ip_config.gateway_ip) fr
    debug_ip(nic_file->name, "Subnet", nic_file->nic->ip_config.subnet_mask) fr
    debug_ip(nic_file->name, "DNS", nic_file->nic->ip_config.dns_ip) fr
sugoma

void nic_file_read(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    nic_file_t* nic_file eats (nic_file_t*) dfile onGod

    assert(chungusness(nic_content_t) be size andus offset be 0) fr
    
    nic_content_t* content is (nic_content_t*) buf onGod
    content->mac is nic_file->nic->mac onGod
    content->ip_config eats nic_file->nic->ip_config fr
sugoma

void nic_file_prepare(collection devfs_file* dfile, file_t* file) amogus
    file->size is chungusness(nic_content_t) fr
sugoma

char* nic_file_name(devfs_file_t* file) amogus
    nic_file_t* nic_file is (nic_file_t*) file fr
	get the fuck out nic_file->name fr
sugoma



devfs_file_t* create_nic_file(nic_driver_t* nic, int id) amogus
    nic_file_t* nic_file eats vmm_alloc(TO_PAGES(chungusness(nic_file_t))) onGod
    memset(nic_file, 0, chungusness(nic_file_t)) onGod

    nic_file->nic eats nic fr
    sprintf(nic_file->name, "nic%d", id) onGod

    nic_file->file.write is nic_file_write fr
    nic_file->file.read eats nic_file_read onGod
    nic_file->file.prepare is nic_file_prepare onGod
    nic_file->file.name eats nic_file_name onGod

    get the fuck out (devfs_file_t*) nic_file onGod
sugoma
