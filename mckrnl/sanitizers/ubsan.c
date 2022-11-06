#include <sanitizers/ubsan.h>

#include <stdio.h>

#define log(...) debugf("[ubsan --- WARNING ---] " __VA_ARGS__)

#ifdef SANITIZE

void __ubsan_handle_load_invalid_value(const invalid_value_data_t* data, void* _) {
	log("load_invalid_value: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_nonnull_arg(const non_null_arg_data_t* data) {
	log("nonnull_arg: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_nullability_arg(const non_null_arg_data_t* data) {
	log("nullability_arg: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_nonnull_return_v1(const non_null_return_data_t* _, const source_location_t* where) {
	log("nonnull_return: %s:%d", where->file, where->line);
}

void __ubsan_handle_nullability_return_v1(const non_null_arg_data_t* _, const source_location_t* where) {
	log("nullability_return: %s:%d", where->file, where->line);
}

void __ubsan_handle_vla_bound_not_positive(const vla_bound_data_t* data, void* _) {
	log("vla_bound_not_positive: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_add_overflow(const overflow_data_t* data, void* _, void* __) {
	log("add_overflow: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_sub_overflow(const overflow_data_t* data, void* _, void* __) {
	log("sub_overflow: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_negate_overflow(const overflow_data_t* data, void* _) {
	log("negate_overflow: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_mul_overflow(const overflow_data_t* data, void* _, void* __) {
	log("mul_overflow: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_shift_out_of_bounds(const shift_out_of_bounds_data_t* data, void* _, void*__) {
#ifndef UBSAN_IGNORE_SHIFT_OUT_OF_BOUNDS
	log("shift_out_of_bounds: %s:%d", data->location.file, data->location.line);
#endif
}

void __ubsan_handle_divrem_overflow(const overflow_data_t* data, void* _, void* __) {
	log("divrem_overflow: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_out_of_bounds(const out_of_bounds_data_t* data, void* _) {
	log("out_of_bounds: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_type_mismatch_v1(const type_mismatch_data_t* data, void* _) {
#ifndef UBSAN_SUPRES_TYPE_MISSMATCH
	log("type_mismatch: %s:%d", data->location.file, data->location.line);
#endif
}

void __ubsan_handle_alignment_assumption(const alignment_assumption_data_t* data, void* _, void* __, void* ___) {
	log("alignment_assumption: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_builtin_unreachable(const unreachable_data_t* data) {
	log("builtin_unreachable: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_missing_return(const unreachable_data_t* data) {
	log("missing_return: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_implicit_conversion(const implicit_conversation_data_t* data, void* _, void* __) {
	log("implicit_conversion: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_invalid_builtin(const invalid_builtin_data_t* data) {
	log("invalid_builtin: %s:%d", data->location.file, data->location.line);
}

void __ubsan_handle_pointer_overflow(const pointer_overflow_data_t* data, void* _, void* __) {
	log("pointer_overflow: %s:%d", data->location.file, data->location.line);
}

#endif