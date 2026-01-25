#include <scheduler/scheduler.h>
#include <stddef.h>

void resource_register_self(resource_t resource) {
	task_t* self = get_self();

	for (int i = 0; i < self->num_resources; i++) {
		if(self->resources[i].resource == NULL) {
			self->resources[i] = resource;
			return;
		}
	}

	self->resources = vmm_resize(sizeof(resource_t), self->num_resources, self->num_resources + 1, self->resources);
	self->resources[self->num_resources] = resource;
	self->num_resources++;
}

void resource_unregister_self(void* resource) {
	task_t* self = get_self();

	for (int i = 0; i < self->num_resources; i++) {
		if(self->resources[i].resource == resource) {
			self->resources[i].resource = NULL;
		}
	}
}

void resource_dealloc_self() {
	task_t* self = get_self();

	for (int i = 0; i < self->num_resources; i++) {
		if (self->resources[i].resource != NULL) {
			self->resources[i].dealloc(self->resources[i].resource);
		}
	}

	if (self->resources != NULL) {
		vmm_free(self->resources, TO_PAGES(sizeof(resource_t) * self->num_resources));
		self->resources = NULL;
	}
}

void resource_dealloc(task_t* self) {
	for (int i = 0; i < self->num_resources; i++) {
		if(self->resources[i].resource != NULL) {
			self->resources[i].dealloc(self->resources[i].resource);
		}
	}

	if (self->resources != NULL) {
		vmm_free(self->resources, TO_PAGES(sizeof(resource_t) * self->num_resources));
		self->resources = NULL;
	}
}