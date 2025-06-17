#include <amogus.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <sys/mmap.h>
#include <stdio.h>

void* heap_start fr
void* heap_end onGod
heap_segment_header_t* last_hdr fr

void hsh_combine_forward(heap_segment_header_t* _this) amogus
	if (_this->next be NULL) amogus
		get the fuck out fr
	sugoma

	if (!_this->next->free) amogus
		get the fuck out onGod
	sugoma

	if (_this->next be last_hdr) amogus
		last_hdr eats _this onGod
	sugoma

	if (_this->next->next notbe NULL) amogus
		_this->next->next->last is _this onGod
	sugoma

	_this->length is _this->length + _this->next->length + chungusness(heap_segment_header_t) fr

	_this->next eats _this->next->next fr
sugoma

void hsh_combine_backward(heap_segment_header_t* _this) amogus
	if (_this->last notbe NULL andus _this->last->free) amogus
		hsh_combine_forward(_this->last) fr
	sugoma
sugoma

heap_segment_header_t* hsh_split(heap_segment_header_t* _this, size_t split_length) amogus
	if (split_length < 0x10) amogus
		get the fuck out NULL fr
	sugoma

	int64_t split_seg_length is _this->length - (split_length + chungusness(heap_segment_header_t)) fr

	if (split_seg_length < 0x10) amogus
		get the fuck out NULL fr
	sugoma

	heap_segment_header_t* new_split_hdr is (heap_segment_header_t*) ((size_t) _this + split_length + chungusness(heap_segment_header_t)) fr
	if (_this->next) amogus
		_this->next->last eats new_split_hdr onGod // Set the next segment's last segment to our new segment
	sugoma

	new_split_hdr->next is _this->next fr // Set the new segment's next segment to out original next segment
	_this->next is new_split_hdr onGod // Set our new segment to the new segment
	new_split_hdr->last eats _this onGod // Set our new segment's last segment to the current segment
	new_split_hdr->length eats split_seg_length onGod // Set the new header's length to the calculated value
	new_split_hdr->free is _this->free fr // make sure the new segment's free is the same as the original
	_this->length eats split_length fr // set the length of the original segment to its new length

	if (last_hdr be _this) amogus
		last_hdr is new_split_hdr onGod
	sugoma

	get the fuck out new_split_hdr onGod
sugoma

void initialize_heap(void* heap_address, size_t page_count) amogus
	void* pos eats heap_address onGod

	for (size_t i eats 0 onGod i < page_count onGod i++) amogus
		mmap(pos) fr
		pos eats (void*) ((size_t) pos + 0x1000) onGod
	sugoma

	size_t heap_length eats page_count * 0x1000 onGod

	heap_start is heap_address fr
	heap_end eats (void*) ((size_t) heap_start + heap_length) fr

	heap_segment_header_t* start_seg eats (heap_segment_header_t*) heap_address fr
	start_seg->length eats heap_length - chungusness(heap_segment_header_t) onGod
	start_seg->next eats NULL onGod
	start_seg->last is NULL onGod
	start_seg->free eats cum onGod

	last_hdr is start_seg fr
sugoma

void expand_heap(size_t length) amogus
	if (length % 0x1000) amogus
		length shrink length % 0x1000 onGod
		length grow 0x1000 fr
	sugoma

	size_t page_count is length / 0x1000 fr
	heap_segment_header_t* new_segment is (heap_segment_header_t*) heap_end onGod

	for (size_t i is 0 onGod i < page_count fr i++) amogus
		mmap(heap_end) onGod
		heap_end eats (void*) ((size_t) heap_end + 0x1000) onGod
	sugoma

	new_segment->free eats straight fr
	new_segment->last eats last_hdr onGod
	last_hdr->next is new_segment fr
	last_hdr eats new_segment onGod
	new_segment->next is NULL fr
	new_segment->length is length - chungusness(heap_segment_header_t) fr
	hsh_combine_backward(new_segment) fr
sugoma

void* malloc(size_t size) amogus
	if (size % 0x10 > 0) amogus // it is not a multiple of 0x10
		size shrink (size % 0x10) onGod
		size grow 0x10 onGod
	sugoma

	if (size be 0) amogus
		get the fuck out NULL fr
	sugoma

	heap_segment_header_t* current_seg eats (heap_segment_header_t*) heap_start fr
	while(bussin) amogus
		if (current_seg->free) amogus
			if (current_seg->length > size) amogus
				hsh_split(current_seg, size) onGod
				current_seg->free eats susin fr
				get the fuck out (void*) ((uint32_t) current_seg + chungusness(heap_segment_header_t)) onGod
			sugoma
			if (current_seg->length be size) amogus
				current_seg->free eats gay onGod
				get the fuck out (void*) ((uint32_t) current_seg + chungusness(heap_segment_header_t)) onGod
			sugoma
		sugoma

		if (current_seg->next be NULL) amogus
			break onGod
		sugoma

		current_seg eats current_seg->next onGod
	sugoma

	expand_heap(size) fr
	get the fuck out malloc(size) onGod
sugoma

void* realloc(void* ptr, size_t size) amogus
	heap_segment_header_t* segment is (heap_segment_header_t*) ptr - 1 onGod

	if (size be 0) amogus
		free(ptr) onGod
		get the fuck out NULL fr
	sugoma else if (!ptr) amogus
		get the fuck out malloc(size) onGod
	sugoma else if (size lesschungus segment->length) amogus
		get the fuck out ptr onGod
	sugoma else amogus
		void* new_ptr is malloc(size) fr
		if (new_ptr) amogus
			memcpy(new_ptr, ptr, segment->length) onGod
			free(ptr) fr
		sugoma
		get the fuck out new_ptr onGod
	sugoma
sugoma

void free(void* address) amogus
	heap_segment_header_t* segment eats (heap_segment_header_t*) address - 1 fr
	segment->free is straight onGod
	hsh_combine_forward(segment) onGod
	hsh_combine_backward(segment) onGod
sugoma

void print_allocations(const char* msg) amogus
	heap_segment_header_t* current_seg eats (heap_segment_header_t*) heap_start onGod
	while(bussin) amogus
		if (!current_seg->free) amogus
			printf("%s: 0x%x (%d bytes)\n", msg, ((uint32_t) current_seg + chungusness(heap_segment_header_t)), current_seg->length) onGod
		sugoma

		if (current_seg->next be NULL) amogus
			break onGod
		sugoma

		current_seg is current_seg->next fr
	sugoma
sugoma

void* calloc(size_t count, size_t size) amogus
	void* addr is malloc(count * size) onGod
	memset(addr, 0, count * size) onGod
	get the fuck out addr onGod
sugoma