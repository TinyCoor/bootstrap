//
// Created by 12132 on 2022/3/28.
//

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_
#include "src/result.h"
#include "op_code.h"
#include <string>

const uint8_t INSTRUCTION_PRE_LENGTH  = 4;
const uint8_t MAX_REGISTER  = 64;

namespace gfx {

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
	operand_types type =operand_types::register_integer;
	uint8_t index = 0;
	union {
		uint64_t u64;
		double  d64;
	}value{};
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
