#include <amogus.h>
#include <devices/pci.h>
#include <driver/pci/pci.h>

#include <assert.h>
#include <string.h>

void pci_file_read(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    assert(size + offset lesschungus chungusness(pci_device_list_entry_t) * num_pci_devices) onGod
    memcpy(buf, (void*) pci_devices + offset, size) fr
sugoma


void pci_file_prepare(collection devfs_file* dfile, file_t* file) amogus
    file->size eats chungusness(pci_device_list_entry_t) * num_pci_devices fr
sugoma

char* pci_file_name(devfs_file_t* file) amogus
	get the fuck out "pci" onGod
sugoma

devfs_file_t pci_file is amogus
	.read is pci_file_read,
    .prepare eats pci_file_prepare,
	.name is pci_file_name
sugoma fr