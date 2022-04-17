//
// Created by 12132 on 2022/4/15.
//

#ifndef BOOTSTRAP_SRC_VM_FFI_H_
#define BOOTSTRAP_SRC_VM_FFI_H_
#include <dyncall/dyncall.h>
#include <dynload/dynload.h>
#include <dyncall/dyncall_signature.h>
#include <dyncall/dyncall_struct.h>

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
	std::string name;
	ffi_types_t type;
	std::vector<function_value_t> fields;

	DCstruct * to_dc_struct() {
		auto dc_struct = dcNewStruct(fields.size(), sizeof(uint64_t));
		add_struct_fields(dc_struct);
		dcCloseStruct(dc_struct);
		return dc_struct;
	}

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

};

class shared_library;

struct function_signature_t {
	std::string symbol;
	void *func_ptr = nullptr;
	shared_library* library = nullptr;
	function_value_t return_value {};
	std::vector<function_value_t> arguments;
	ffi_calling_mode_t calling_mode = ffi_calling_mode_t::c_default;

};
///////////////////////////////////////////////////////////////////////////
}
#endif //BOOTSTRAP_SRC_VM_FFI_H_
