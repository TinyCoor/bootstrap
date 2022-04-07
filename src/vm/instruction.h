//
// Created by 12132 on 2022/3/28.
//

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_
#include "src/common/result.h"
#include "op_code.h"
#include <string>

const uint8_t MAX_REGISTER  = 64;
namespace gfx {

enum i_registers_t : uint8_t {
	i0,
	i1,
	i2,
	i3,
	i4,
	i5,
	i6,
	i7,
	i8,
	i9,

	i10,
	i11,
	i12,
	i13,
	i14,
	i15,
	i16,
	i17,
	i18,
	i19,

	i20,
	i21,
	i22,
	i23,
	i24,
	i25,
	i26,
	i27,
	i28,
	i29,

	i30,
	i31,
	i32,
	i33,
	i34,
	i35,
	i36,
	i37,
	i38,
	i39,

	i40,
	i41,
	i42,
	i43,
	i44,
	i45,
	i46,
	i47,
	i48,
	i49,

	i50,
	i51,
	i52,
	i53,
	i54,
	i55,
	i56,
	i57,
	i58,
	i59,

	i60,
	i61,
	i62,
	i63,

	pc,
	sp,
	fr,
	sr
};

enum f_registers_t : uint8_t {
	f0,
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,

	f10,
	f11,
	f12,
	f13,
	f14,
	f15,
	f16,
	f17,
	f18,
	f19,

	f20,
	f21,
	f22,
	f23,
	f24,
	f25,
	f26,
	f27,
	f28,
	f29,

	f30,
	f31,
	f32,
	f33,
	f34,
	f35,
	f36,
	f37,
	f38,
	f39,

	f40,
	f41,
	f42,
	f43,
	f44,
	f45,
	f46,
	f47,
	f48,
	f49,

	f50,
	f51,
	f52,
	f53,
	f54,
	f55,
	f56,
	f57,
	f58,
	f59,

	f60,
	f61,
	f62,
	f63,
};

struct register_file_t {
	enum flags_t : uint64_t {
		zero     = 0b0000000000000000000000000000000000000000000000000000000000000001,
		carry    = 0b0000000000000000000000000000000000000000000000000000000000000010,
		overflow = 0b0000000000000000000000000000000000000000000000000000000000000100,
		negative = 0b0000000000000000000000000000000000000000000000000000000000001000,
		extended = 0b0000000000000000000000000000000000000000000000000000000000010000,
	};

	bool flags(flags_t f) const {
		return (fr & f) != 0;
	}

	void flags(flags_t f, bool value) {
		if (value)
			fr |= f;
		else
			fr &= ~f;
	}
	uint64_t i[MAX_REGISTER];
	double f[MAX_REGISTER];
	uint64_t pc;
	uint64_t sp;
	uint64_t fr;
	uint64_t sr;
};

enum class op_sizes : uint8_t {
	none,
	byte,
	word,
	dword,
	qword,
};

enum class operand_types : uint8_t {
	register_integer = 1,
	register_floating_point,
	register_pc,
	register_sp,
	register_flags,
	register_status,
	constant_integer,
	constant_float,
	constant_offset_positive,
	constant_offset_negative,
	increment_constant_pre,
	increment_constant_post,
	increment_register_pre,
	increment_register_post,
	decrement_constant_pre,
	decrement_constant_post,
	decrement_register_pre,
	decrement_register_post
};

struct operand_encoding_t {
	using flags_t = uint8_t;

	enum flags : uint8_t {
		none     = 0b00000000,
		constant = 0b00000000,
		reg      = 0b00000001,
		integer  = 0b00000010,
		negative = 0b00000100,
		prefix   = 0b00001000,
		postfix  = 0b00010000,
	};
	inline bool is_reg() const {
		return (type & flags::reg) != 0;
	}

	inline bool is_prefix() const {
		return (type & flags::prefix) != 0;
	}

	inline bool is_postfix() const {
		return (type & flags::postfix) != 0;
	}

	inline bool is_integer() const {
		return (type & flags::integer) != 0;
	}

	inline bool is_negative() const {
		return (type & flags::negative) != 0;
	}
	flags_t type = flags::reg | flags::integer;
	union {
		uint8_t r8 = 0;
		uint64_t u64;
		double  d64;
	}value;
};

struct debug_information_t {
	uint32_t line_number;
	int16_t column_number;
	std::string symbol;
	std::string source_file;
};

/// todo separate instruction by operand count
/// unary instruction
/// binary instruction
struct instruction_t {
	static constexpr size_t base_size = 3;
	static constexpr size_t alignment = 4;

	size_t encode(result& r, uint8_t* heap, uint64_t address);

	size_t decode(result& r, uint8_t* heap, uint64_t address);

	size_t align(uint64_t value, size_t size) const;

	size_t encoding_size() const;

	void patch_branch_address(uint64_t address, uint8_t index = 0u);

	op_codes op = op_codes::nop;
	op_sizes size = op_sizes::none;
	uint8_t operands_count = 0;
	operand_encoding_t operands[4];
};
}

#endif // INSTRUCTION_H_
