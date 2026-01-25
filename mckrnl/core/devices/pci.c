#include <devices/pci.h>
#include <driver/pci/pci.h>

#include <assert.h>
#include <string.h>

void pci_file_read(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    assert(size + offset <= sizeof(pci_device_list_entry_t) * num_pci_devices);
    memcpy(buf, (void*) pci_devices + offset, size);
}


void pci_file_prepare(struct devfs_file* dfile, file_t* file) {
    file->size = sizeof(pci_device_list_entry_t) * num_pci_devices;
}

char* pci_file_name(devfs_file_t* file) {
	return "pci";
}

devfs_file_t pci_file = {
	.read = pci_file_read,
    .prepare = pci_file_prepare,
	.name = pci_file_name
};