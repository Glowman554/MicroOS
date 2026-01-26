#include <acpi_vfs.h>
#include <driver/disk_driver.h>
#include <memory/heap.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

acpi_vfs_mount_data_t create_acpi_vfs_mount_data() {
    int entriesXSDT = xsdt ? (xsdt->header.length - sizeof(sdt_header_t)) / 8 : 0;
	int entriesRSDT = rsdt ? (rsdt->header.length - sizeof(sdt_header_t)) / 4 : 0;

	acpi_vfs_mount_data_t mount_data;
	mount_data.table_count = entriesXSDT + entriesRSDT;
	mount_data.tables = (sdt_header_t**) kmalloc(sizeof(sdt_header_t*) * mount_data.table_count);

	if(xsdt != NULL) {
		for(uint64_t i = 0; i < entriesXSDT; i++) {
			sdt_header_t* acpihdr = (sdt_header_t*) (uint32_t) xsdt->acpiptr[i];
            debugf("[XSDT] Found ACPI table: %c%c%c%c", acpihdr->signature[0], acpihdr->signature[1], acpihdr->signature[2], acpihdr->signature[3]);
			mount_data.tables[i] = acpihdr;
        }
	} 

	if(rsdt != NULL) {
		for(uint64_t i = 0; i < entriesRSDT; i++) {
			sdt_header_t* acpihdr = (sdt_header_t*) (rsdt->acpiptr[i]);
            debugf("[RSDT] Found ACPI table: %c%c%c%c", acpihdr->signature[0], acpihdr->signature[1], acpihdr->signature[2], acpihdr->signature[3]);
			mount_data.tables[entriesXSDT + i] = acpihdr;
		}
	} 

	return mount_data;
}

char* acpi_vfs_name(vfs_mount_t* mount) {
    return "acpi";
}

file_t* acpi_vfs_open(vfs_mount_t* mount, char* path, int flags) {
	acpi_vfs_mount_data_t* data = (acpi_vfs_mount_data_t*) mount->driver_specific_data;

    char file[0xff] = { 0 };
    while (*(path = copy_until('/', path, file)));
    debugf("file: %s", file);

	for (size_t i = 0; i < data->table_count; i++) {
		sdt_header_t* table = data->tables[i];
		char expected_name[0xff] = { 0 };
		sprintf(expected_name, "%d.%c%c%c%c", i, table->signature[0], table->signature[1], table->signature[2], table->signature[3]);
	
		if (strcmp(file, expected_name) == 0) {
			file_t* file = (file_t*) kmalloc(sizeof(file_t));
            memset(file, 0, sizeof(file_t));

            file->mount = mount;
            file->size = table->length;
			file->driver_specific_data = table;

			return file;
		}
	}

    return NULL;
}

void acpi_vfs_close(vfs_mount_t* mount, file_t* f) {
	kfree(f);
}

void acpi_vfs_read(vfs_mount_t* mount, file_t* f, void* buffer, size_t size, size_t offset) {
	if (offset + size > f->size) {
		abortf(true, "Attempted to read beyond end of ACPI table file");
	}
	memcpy(buffer, &((char*) f->driver_specific_data)[offset], size);
}


dir_t acpi_vfs_dir_at(vfs_mount_t* mount, int idx, char* path) {
	acpi_vfs_mount_data_t* mount_data = (acpi_vfs_mount_data_t*) mount->driver_specific_data;
	if(idx >= mount_data->table_count) {
		dir_t empty = {
			.is_none = true
		};
		return empty;
	}

	sdt_header_t* table = mount_data->tables[idx];
	dir_t entry = {
		.idx = idx,
		.is_none = false,
		.type = ENTRY_FILE
	};
	sprintf(entry.name, "%d.%c%c%c%c", idx, table->signature[0], table->signature[1], table->signature[2], table->signature[3]);

	return entry;
}

vfs_mount_t* acpi_vfs_mount() {
    vfs_mount_t* mount = (vfs_mount_t*) kmalloc(sizeof(vfs_mount_t) + sizeof(acpi_vfs_mount_data_t));
	memset(mount, 0, sizeof(vfs_mount_t) + sizeof(acpi_vfs_mount_data_t));

	acpi_vfs_mount_data_t* mount_data = (acpi_vfs_mount_data_t*) &mount[1];
	mount->driver_specific_data = mount_data;

	*mount_data = create_acpi_vfs_mount_data();

	mount->name = acpi_vfs_name;
	mount->open = acpi_vfs_open;
	mount->close = acpi_vfs_close;
	mount->read = acpi_vfs_read;
	mount->dir_at = acpi_vfs_dir_at;

	return mount;
}