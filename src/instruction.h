//
// Created by 12132 on 2022/3/28.
//

#ifndef BOOTSTRAP__INSTRUCTION_H_
#define BOOTSTRAP__INSTRUCTION_H_
#include <cstdint>
#include <string>
namespace gfx {

struct register_file_t {
	uint64_t i[64];
	double f[64];
	uint64_t pc;
	uint64_t sp;
	uint64_t fr;
	uint64_t sr;
};

enum class op_codes : uint16_t {
	nop = 1,
	load,
	store,
	move,
	push,
	copy,
	fill,
	pop,
	add,
	sub,
	inc,
	dec,
	mul,
	div,
	mod,
	neg,
	shl,
	shr,
	rol,
	ror,
	and_op,
	or_op,
	xor_op,
	not_op,
	bis,
	bic,
	test,
	cmp,
	bz,
	bnz,
	tbz,
	tbnz,
	bne,
	beq,
	bae,
	ba,
	ble,
	bl,
	bo,
	bcc,
	bcs,
	jsr,
	rts,
	jmp,
	meta,
	debug,
	exit,
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
	}value;
};

struct debug_information_t {
	uint32_t line_number;
	int16_t column_number;
	std::string symbol;
	std::string source_file;
};


struct instruction_t {
	size_t align(uint64_t value, size_t size) const {
		auto offset = value % size;
		return offset ? value + (size - offset) : value;
	}

	size_t encoding_size() const {
		size_t size = 5;

		for (size_t i = 0; i < operands_count; i++) {
			size += 2;
			switch (operands[i].type) {
				case operand_types::increment_constant_pre:
				case operand_types::increment_constant_post:
				case operand_types::decrement_constant_pre:
				case operand_types::decrement_constant_post:
				case operand_types::constant_integer: {
					size += sizeof(uint64_t);
					break;
				}
				case operand_types::constant_float: {
					size += sizeof(double);
					break;
				}
				default:
					break;
			}
		}

		if (size < 8)
			size = 8;

		size = static_cast<uint8_t>(align(size, sizeof(uint64_t)));

		return size;
	}

	void patch_branch_address(uint64_t address) {
		operands[0].value.u64 = address;
	}

	op_codes op = op_codes::nop;
	op_sizes size = op_sizes::none;
	uint8_t operands_count = 0;
	operand_encoding_t operands[4];
};
}

#endif //BOOTSTRAP__INSTRUCTION_H_
