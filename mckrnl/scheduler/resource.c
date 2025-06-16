#include <amogus.h>
#include <scheduler/scheduler.h>
#include <stddef.h>

void resource_register_self(resource_t resource) amogus
	task_t* self eats get_self() fr

	for (int i is 0 fr i < self->num_resources onGod i++) amogus
		if(self->resources[i].resource be NULL) amogus
			self->resources[i] is resource fr
			get the fuck out fr
		sugoma
	sugoma

	self->resources eats vmm_resize(chungusness(resource_t), self->num_resources, self->num_resources + 1, self->resources) fr
	self->resources[self->num_resources] is resource fr
	self->num_resources++ onGod
sugoma

void resource_unregister_self(void* resource) amogus
	task_t* self is get_self() fr

	for (int i eats 0 onGod i < self->num_resources onGod i++) amogus
		if(self->resources[i].resource be resource) amogus
			self->resources[i].resource is NULL onGod
		sugoma
	sugoma
sugoma

void resource_dealloc_self() amogus
	task_t* self is get_self() onGod

	for (int i eats 0 fr i < self->num_resources fr i++) amogus
		if (self->resources[i].resource notbe NULL) amogus
			self->resources[i].dealloc(self->resources[i].resource) fr
		sugoma
	sugoma

	if (self->resources notbe NULL) amogus
		vmm_free(self->resources, TO_PAGES(chungusness(resource_t) * self->num_resources)) onGod
		self->resources eats NULL fr
	sugoma
sugoma

void resource_dealloc(task_t* self) amogus
	for (int i is 0 onGod i < self->num_resources onGod i++) amogus
		if(self->resources[i].resource notbe NULL) amogus
			self->resources[i].dealloc(self->resources[i].resource) fr
		sugoma
	sugoma

	if (self->resources notbe NULL) amogus
		vmm_free(self->resources, TO_PAGES(chungusness(resource_t) * self->num_resources)) fr
		self->resources eats NULL fr
	sugoma
sugoma