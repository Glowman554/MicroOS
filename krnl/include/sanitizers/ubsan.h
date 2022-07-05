#pragma once

#include <stdint.h>

typedef struct source_location_t {
	const char* file;
	uint32_t line;
	uint32_t column;
} source_location_t;

typedef struct type_descriptor_t {
	uint16_t kind;
	uint16_t info;
	char* name;
} type_descriptor_t;

typedef struct invalid_value_data_t {
	source_location_t location;
	const type_descriptor_t* type;
} invalid_value_data_t;

typedef struct non_null_arg_data_t {
	source_location_t location;
	source_location_t attribute_location;
	int argument_index;
} non_null_arg_data_t;

typedef struct non_null_return_data_t {
	source_location_t location;
} non_null_return_data_t;

typedef struct overflow_data_t {
	source_location_t location;
	const type_descriptor_t* type_descriptor;
} overflow_data_t;

typedef struct vla_bound_data_t {
	source_location_t location;
	const type_descriptor_t* type_descriptor;
} vla_bound_data_t;

typedef struct shift_out_of_bounds_data_t {
	source_location_t location;
	const type_descriptor_t* left_operand;
	const type_descriptor_t* right_operand;
} shift_out_of_bounds_data_t;

typedef struct out_of_bounds_data_t {
	source_location_t location;
	const type_descriptor_t* array_type;
	const type_descriptor_t* index_type;
} out_of_bounds_data_t;

typedef struct type_mismatch_data_t {
	source_location_t location;
	const type_descriptor_t* type;
	uint8_t log_alignment;
	uint8_t type_check_kind;
} type_mismatch_data_t;

typedef struct alignment_assumption_data_t {
	source_location_t location;
	source_location_t assumption_location;
	const type_descriptor_t* type;
} alignment_assumption_data_t;

typedef struct unreachable_data_t {
	source_location_t location;
} unreachable_data_t;

typedef struct implicit_conversation_data_t {
	source_location_t location;
	const type_descriptor_t* from_type;
	const type_descriptor_t* to_type;
	uint8_t kind;
} implicit_conversation_data_t;

typedef struct invalid_builtin_data_t {
	source_location_t location;
	uint8_t kind;
} invalid_builtin_data_t;

typedef struct pointer_overflow_data_t {
	source_location_t location;
} pointer_overflow_data_t;

typedef struct float_cat_overflow_data_t {
	source_location_t location;
	const type_descriptor_t* from_type;
	const type_descriptor_t* to_type;
} float_cat_overflow_data_t;


void __ubsan_handle_load_invalid_value(const invalid_value_data_t* data, void*);
void __ubsan_handle_nonnull_arg(const non_null_arg_data_t* data);
void __ubsan_handle_nullability_arg(const non_null_arg_data_t* data);
void __ubsan_handle_nonnull_return_v1(const non_null_return_data_t*, const source_location_t* where);
void __ubsan_handle_nullability_return_v1(const non_null_arg_data_t*, const source_location_t* where);
void __ubsan_handle_vla_bound_not_positive(const vla_bound_data_t* data, void*);
void __ubsan_handle_add_overflow(const overflow_data_t* data, void*, void*);
void __ubsan_handle_sub_overflow(const overflow_data_t* data, void*, void*);
void __ubsan_handle_negate_overflow(const overflow_data_t* data, void*);
void __ubsan_handle_mul_overflow(const overflow_data_t* data, void*, void*);
void __ubsan_handle_shift_out_of_bounds(const shift_out_of_bounds_data_t* data, void*, void*);
void __ubsan_handle_divrem_overflow(const overflow_data_t* data, void*, void*);
void __ubsan_handle_out_of_bounds(const out_of_bounds_data_t* data, void*);
void __ubsan_handle_type_mismatch_v1(const type_mismatch_data_t* data, void*);
void __ubsan_handle_alignment_assumption(const alignment_assumption_data_t* data, void*, void*, void*);
void __ubsan_handle_builtin_unreachable(const unreachable_data_t* data);
void __ubsan_handle_missing_return(const unreachable_data_t* data);
void __ubsan_handle_implicit_conversion(const implicit_conversation_data_t* data, void*, void*);
void __ubsan_handle_invalid_builtin(const invalid_builtin_data_t* data);
void __ubsan_handle_pointer_overflow(const pointer_overflow_data_t* data, void*, void*);