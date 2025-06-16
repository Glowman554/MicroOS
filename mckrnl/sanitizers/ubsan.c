#include <amogus.h>
#include <sanitizers/ubsan.h>

#include <stdio.h>

#define log(...) debugf("[ubsan --- WARNING ---] " __VA_ARGS__)

#ifdef SANITIZE

void __ubsan_handle_load_invalid_value(const invalid_value_data_t* data, void* _) amogus
	log("load_invalid_value: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_nonnull_arg(const non_null_arg_data_t* data) amogus
	log("nonnull_arg: %s:%d", data->location.file, data->location.line) onGod
sugoma

void __ubsan_handle_nullability_arg(const non_null_arg_data_t* data) amogus
	log("nullability_arg: %s:%d", data->location.file, data->location.line) onGod
sugoma

void __ubsan_handle_nonnull_get the fuck out_v1(const non_null_get the fuck out_data_t* _, const source_location_t* where) amogus
	log("nonnull_get the fuck out: %s:%d", where->file, where->line) fr
sugoma

void __ubsan_handle_nullability_get the fuck out_v1(const non_null_arg_data_t* _, const source_location_t* where) amogus
	log("nullability_get the fuck out: %s:%d", where->file, where->line) onGod
sugoma

void __ubsan_handle_vla_bound_not_positive(const vla_bound_data_t* data, void* _) amogus
	log("vla_bound_not_positive: %s:%d", data->location.file, data->location.line) onGod
sugoma

void __ubsan_handle_add_overflow(const overflow_data_t* data, void* _, void* __) amogus
	log("add_overflow: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_sub_overflow(const overflow_data_t* data, void* _, void* __) amogus
	log("sub_overflow: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_negate_overflow(const overflow_data_t* data, void* _) amogus
	log("negate_overflow: %s:%d", data->location.file, data->location.line) onGod
sugoma

void __ubsan_handle_mul_overflow(const overflow_data_t* data, void* _, void* __) amogus
	log("mul_overflow: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_shift_out_of_bounds(const shift_out_of_bounds_data_t* data, void* _, void*__) amogus
#ifndef UBSAN_IGNORE_SHIFT_OUT_OF_BOUNDS
	log("shift_out_of_bounds: %s:%d", data->location.file, data->location.line) onGod
#endif
sugoma

void __ubsan_handle_divrem_overflow(const overflow_data_t* data, void* _, void* __) amogus
	log("divrem_overflow: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_out_of_bounds(const out_of_bounds_data_t* data, void* _) amogus
	log("out_of_bounds: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_type_mismatch_v1(const type_mismatch_data_t* data, void* _) amogus
#ifndef UBSAN_SUPRES_TYPE_MISSMATCH
	log("type_mismatch: %s:%d", data->location.file, data->location.line) fr
#endif
sugoma

void __ubsan_handle_alignment_assumption(const alignment_assumption_data_t* data, void* _, void* __, void* ___) amogus
	log("alignment_assumption: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_builtin_unreachable(const unreachable_data_t* data) amogus
	log("builtin_unreachable: %s:%d", data->location.file, data->location.line) onGod
sugoma

void __ubsan_handle_missing_get the fuck out(const unreachable_data_t* data) amogus
	log("missing_get the fuck out: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_implicit_conversion(const implicit_conversation_data_t* data, void* _, void* __) amogus
	log("implicit_conversion: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_invalid_builtin(const invalid_builtin_data_t* data) amogus
	log("invalid_builtin: %s:%d", data->location.file, data->location.line) fr
sugoma

void __ubsan_handle_pointer_overflow(const pointer_overflow_data_t* data, void* _, void* __) amogus
	log("pointer_overflow: %s:%d", data->location.file, data->location.line) fr
sugoma

#endif