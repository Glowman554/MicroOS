#include <memory/heap.h>
#include <memory/vmm.h>
#include <memory/pmm.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <utils/trace.h>

void* heap_start;
void* heap_end;
heap_segment_header_t* last_hdr;

void hsh_combine_forward(heap_segment_header_t* _this) {
	if (_this->next == NULL) {
		return;
	}

	if (!_this->next->free) {
		return;
	}

	if (_this->next == last_hdr) {
		last_hdr = _this;
	}

	if (_this->next->next != NULL) {
		_this->next->next->last = _this;
	}

	_this->length = _this->length + _this->next->length + sizeof(heap_segment_header_t);

	_this->next = _this->next->next;
}

void hsh_combine_backward(heap_segment_header_t* _this) {
	if (_this->last != NULL && _this->last->free) {
		hsh_combine_forward(_this->last);
	}
}

heap_segment_header_t* hsh_split(heap_segment_header_t* _this, size_t split_length) {
	if (split_length < 0x10) {
		return NULL;
	}

	int64_t split_seg_length = _this->length - (split_length + sizeof(heap_segment_header_t));

	if (split_seg_length < 0x10) {
		return NULL;
	}

	heap_segment_header_t* new_split_hdr = (heap_segment_header_t*) ((size_t) _this + split_length + sizeof(heap_segment_header_t));
	if (_this->next) {
		_this->next->last = new_split_hdr; // Set the next segment's last segment to our new segment
	}

	new_split_hdr->next = _this->next; // Set the new segment's next segment to out original next segment
	_this->next = new_split_hdr; // Set our new segment to the new segment
	new_split_hdr->last = _this; // Set our new segment's last segment to the current segment
	new_split_hdr->length = split_seg_length; // Set the new header's length to the calculated value
	new_split_hdr->free = _this->free; // make sure the new segment's free is the same as the original
	_this->length = split_length; // set the length of the original segment to its new length

	if (last_hdr == _this) {
		last_hdr = new_split_hdr;
	}

	return new_split_hdr;
}

void initialize_heap(size_t page_count) {
	size_t heap_length = page_count * 0x1000;

	heap_start = vmm_alloc(page_count);
	heap_end = (void*) ((size_t) heap_start + heap_length);

    debugf("Heap initialized from 0x%x to 0x%x (%d pages, %d bytes)", heap_start, heap_end, page_count, heap_length);

	heap_segment_header_t* start_seg = (heap_segment_header_t*) heap_start;
	start_seg->length = heap_length - sizeof(heap_segment_header_t);
	start_seg->next = NULL;
	start_seg->last = NULL;
	start_seg->free = true;

	last_hdr = start_seg;
}

void print_allocations(const char* msg) {
	heap_segment_header_t* current_seg = (heap_segment_header_t*) heap_start;

	debugf("--- Heap allocations: %s ---", msg);
	while(true) {
		debugf("0x%x (%d bytes) free: %s", ((uint32_t) current_seg + sizeof(heap_segment_header_t)), current_seg->length, current_seg->free ? "true" : "false");

		if (current_seg->next == NULL) {
			break;
		}

		current_seg = current_seg->next;
	}
	debugf("--- End of heap allocations ---");
}

#if 0
void expand_heap(size_t length) {
	print_allocations("Before expanding heap");
	if (length % 0x1000) {
		length -= length % 0x1000;
		length += 0x1000;
	}

	size_t page_count = length / 0x1000;
	heap_segment_header_t* new_segment = (heap_segment_header_t*) heap_end;

	for (size_t i = 0; i < page_count; i++) {
		void* physical_page = pmm_alloc();
        vmm_map_page(kernel_context, (uintptr_t) heap_end, (uintptr_t) physical_page, PTE_PRESENT | PTE_WRITE);

        debugf("Mapped new heap page: virtual 0x%x to physical 0x%x\n", heap_end, physical_page);

		heap_end = (void*) ((size_t) heap_end + 0x1000);
	}

	new_segment->free = true;
	new_segment->last = last_hdr;
	last_hdr->next = new_segment;
	last_hdr = new_segment;
	new_segment->next = NULL;
	new_segment->length = length - sizeof(heap_segment_header_t);
	hsh_combine_backward(new_segment);
}
#endif

void* kmalloc(size_t size) {
	if (size % 0x10 > 0) { // it is not a multiple of 0x10
		size -= (size % 0x10);
		size += 0x10;
	}

	if (size == 0) {
		return NULL;
	}

	heap_segment_header_t* current_seg = (heap_segment_header_t*) heap_start;
	while(true) {
		if (current_seg->free) {
			if (current_seg->length > size) {
				hsh_split(current_seg, size);
				current_seg->free = false;
				return (void*) ((uint32_t) current_seg + sizeof(heap_segment_header_t));
			}
			if (current_seg->length == size) {
				current_seg->free = false;
				return (void*) ((uint32_t) current_seg + sizeof(heap_segment_header_t));
			}
		}

		if (current_seg->next == NULL) {
			break;
		}

		current_seg = current_seg->next;
	}

#if 0
	expand_heap(size);
	return kmalloc(size);
#else
	print_allocations("Heap full");
	abortf(false, "kernel heap full: could not allocate %d bytes", size);
#endif
}

void* krealloc(void* ptr, size_t size) {
	heap_segment_header_t* segment = (heap_segment_header_t*) ptr - 1;

	if (size == 0) {
		kfree(ptr);
		return NULL;
	} else if (!ptr) {
		return kmalloc(size);
	} else if (size <= segment->length) {
		return ptr;
	} else {
		void* new_ptr = kmalloc(size);
		if (new_ptr) {
			memcpy(new_ptr, ptr, segment->length);
			kfree(ptr);
		}
		return new_ptr;
	}
}

void kfree(void* address) {
	if (address == NULL) {
		return;
	}
	heap_segment_header_t* segment = (heap_segment_header_t*) address - 1;
	segment->free = true;
	hsh_combine_forward(segment);
	hsh_combine_backward(segment);
}

void* kcalloc(size_t count, size_t size) {
	void* addr = kmalloc(count * size);
	memset(addr, 0, count * size);
	return addr;
}