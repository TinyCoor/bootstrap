//
// Created by 12132 on 2022/4/15.
//

#ifndef FFI_H_
#define FFI_H_
#include "dyncall/dyncall.h"
#include "dynload/dynload.h"
#include "dyncall/dyncall_signature.h"
#include "dyncall/dyncall_struct.h"
#include <cstdint>
#include <string>
#include <vector>

namespace gfx {
enum class ffi_calling_mode_t : uint16_t {
	c_default = 1,
	c_ellipsis,
	c_ellipsis_varargs,
};

enum class ffi_types_t : uint16_t {
	void_type = 1,
	bool_type,
	char_type,
	short_type,
	int_type,
	long_type,
	long_long_type,
	float_type,
	double_type,
	pointer_type,
	struct_type,
};

struct function_value_t {
	~function_value_t() {
		if (struct_meta_data_ !=nullptr) {
			delete struct_meta_data_;
			struct_meta_data_ = nullptr;
		}
	}

	DCstruct *struct_meta_info() {
		if (struct_meta_data_ !=nullptr) {
			return struct_meta_data_;
		}
		struct_meta_data_ = dcNewStruct(fields.size(), DEFAULT_ALIGNMENT);
		add_struct_fields(struct_meta_data_);
		dcCloseStruct(struct_meta_data_);
		return struct_meta_data_;
	}

	void push(DCCallVM* vm, uint64_t value){
		switch (type) {
			case ffi_types_t::void_type:
				break;
			case ffi_types_t::bool_type:
				dcArgBool(vm, static_cast<DCbool>(value));
				break;
			case ffi_types_t::char_type:
				dcArgChar(vm, static_cast<DCchar>(value));
				break;
			case ffi_types_t::short_type:
				dcArgShort(vm, static_cast<DCshort>(value));
				break;
			case ffi_types_t::int_type:
				dcArgInt(vm, static_cast<DCint>(value));
				break;
			case ffi_types_t::long_type:
				dcArgLong(vm, static_cast<DClong>(value));
				break;
			case ffi_types_t::long_long_type:
				dcArgLongLong(vm, static_cast<DClonglong>(value));
				break;
			case ffi_types_t::float_type:
				dcArgFloat(vm, static_cast<DCfloat>(value));
				break;
			case ffi_types_t::double_type:
				dcArgDouble(vm, static_cast<DCdouble>(value));
				break;
			case ffi_types_t::pointer_type:
				dcArgPointer(vm, reinterpret_cast<DCpointer>(value));
				break;
			case ffi_types_t::struct_type: {
				auto dc_struct = struct_meta_info();
				dcArgStruct(vm, dc_struct, reinterpret_cast<DCpointer>(value));
				break;
			}
		}
	}

	std::string name;
	ffi_types_t type {};
	std::vector<function_value_t> fields;

private:
	void add_struct_fields(DCstruct* dc_struct) {
		for (auto& value : fields) {
			switch (value.type) {
				case ffi_types_t::void_type: {
					break;
				}
				case ffi_types_t::bool_type: {
					dcStructField(dc_struct, DC_SIGCHAR_BOOL, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::char_type: {
					dcStructField(dc_struct, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::short_type: {
					dcStructField(dc_struct, DC_SIGCHAR_SHORT, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::int_type: {
					dcStructField(dc_struct, DC_SIGCHAR_INT, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::long_type:{
					dcStructField(dc_struct, DC_SIGCHAR_LONG, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::long_long_type: {
					dcStructField(dc_struct, DC_SIGCHAR_LONGLONG, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::float_type:{
					dcStructField(dc_struct, DC_SIGCHAR_FLOAT, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::double_type: {
					dcStructField(dc_struct, DC_SIGCHAR_DOUBLE, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::pointer_type: {
					dcStructField(dc_struct, DC_SIGCHAR_POINTER, DEFAULT_ALIGNMENT, 1);
					break;
				}
				case ffi_types_t::struct_type: {
					dcStructField(dc_struct, DC_SIGCHAR_STRUCT, DEFAULT_ALIGNMENT, 1);
					dcSubStruct(dc_struct, value.fields.size(), DEFAULT_ALIGNMENT,  1);
					value.add_struct_fields(dc_struct);
					dcCloseStruct(dc_struct);
					break;
				}
			}
		}
	}

private:
	DCstruct *struct_meta_data_ = nullptr;
};

class shared_library;

struct function_signature_t {
	void apply_calling_convention(DCCallVM* vm ) const {
		switch (calling_mode) {
			case ffi_calling_mode_t::c_default:
				dcMode(vm, DC_CALL_C_DEFAULT);
				break;
			case ffi_calling_mode_t::c_ellipsis:
				dcMode(vm, DC_CALL_C_ELLIPSIS);
				break;
			case ffi_calling_mode_t::c_ellipsis_varargs:
				dcMode(vm, DC_CALL_C_ELLIPSIS_VARARGS);
				break;
		}
	}

	uint64_t call(DCCallVM* vm, uint64_t address) {
		switch (return_value.type) {
			case ffi_types_t::void_type:
				dcCallVoid(vm, reinterpret_cast<DCpointer>(address));
				break;
			case ffi_types_t::bool_type: {
				auto value = static_cast<uint64_t>(dcCallBool(
					vm,
					reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::char_type: {
				auto value = static_cast<uint64_t>((int64_t)dcCallChar(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::short_type: {
				auto value = static_cast<uint64_t>(dcCallShort(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::int_type: {
				auto value = static_cast<uint64_t>(dcCallInt(
					vm,
					reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::long_type: {
				auto value = static_cast<uint64_t>(dcCallLong(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::long_long_type: {
				auto value = static_cast<uint64_t>(dcCallLongLong(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::float_type: {
				auto value = static_cast<uint64_t>(dcCallFloat(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::double_type: {
				auto value = static_cast<uint64_t>(dcCallDouble(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::pointer_type: {
				auto value = reinterpret_cast<uint64_t>(dcCallPointer(
					vm, reinterpret_cast<DCpointer>(address)));
				return value;
			}
			case ffi_types_t::struct_type: {
				auto dc_struct = return_value.struct_meta_info();

				DCpointer output_value;
				dcCallStruct(vm, reinterpret_cast<DCpointer>(address),
					dc_struct, &output_value);

				return reinterpret_cast<uint64_t>(output_value);
			}
		}
		return 0;
	}

	std::string symbol;
	void *func_ptr = nullptr;
	shared_library* library = nullptr;
	function_value_t return_value {};
	std::vector<function_value_t> arguments;
	ffi_calling_mode_t calling_mode = ffi_calling_mode_t::c_default;
};
}
#endif // FFI_H_
