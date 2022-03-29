//
// Created by 12132 on 2022/3/28.
//

#ifndef BOOTSTRAP__INSTRUCTION_H_
#define BOOTSTRAP__INSTRUCTION_H_
#include <cstdint>
#include <string>
#include "result.h"
namespace gfx {

struct register_file_t {
	enum flags_t : uint64_t {
		zero     = 0b0000000000000000000000000000000000000000000000000000000000000001,
		carry    = 0b0000000000000000000000000000000000000000000000000000000000000010,
		overflow = 0b0000000000000000000000000000000000000000000000000000000000000100,
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
		operands[0].value.u64 = align(address, sizeof(uint64_t));
	}

	size_t encode(result& r, uint8_t* heap, uint64_t address)
	{
		if (address % 8 != 0) {
			r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
			return 0;
		}
		uint8_t encoding_size = 5;

		auto* op_ptr = reinterpret_cast<uint16_t*>(heap + address + 1);
		*op_ptr = static_cast<uint16_t>(op);

		auto encoding_ptr = heap + address;
		*(encoding_ptr + 3) = static_cast<uint8_t>(size);
		*(encoding_ptr + 4) = operands_count;

		auto offset = 5u;
		for (size_t i = 0u; i < operands_count ; ++i) {
			*(encoding_ptr + offset) = static_cast<uint8_t>(operands[i].type);
			++offset;
			++encoding_size;

			*(encoding_ptr + offset) = operands[i].index;
			++offset;
			++encoding_size;
			switch (operands[i].type) {
				case operand_types::register_sp:
				case operand_types::register_pc:
				case operand_types::register_flags:
				case operand_types::register_status:
				case operand_types::register_integer:
				case operand_types::increment_register_pre:
				case operand_types::decrement_register_pre:
				case operand_types::register_floating_point:
				case operand_types::increment_register_post:
				case operand_types::decrement_register_post: break;
				case operand_types::increment_constant_pre:
				case operand_types::increment_constant_post:
				case operand_types::decrement_constant_pre:
				case operand_types::decrement_constant_post:
				case operand_types::constant_integer: {
					uint64_t *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
					*constant_value_ptr = operands[i].value.u64;
					offset += sizeof(uint64_t);
					encoding_size += sizeof(uint64_t);
					break;
				}
				case operand_types::constant_float: {
					double *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
					*constant_value_ptr = operands[i].value.d64;
					offset += sizeof(double);
					encoding_size += sizeof(double);
					break;
				}
			}
		}

		if (encoding_size < 8)
			encoding_size = 8;

		encoding_size = static_cast<uint8_t>(align(encoding_size, sizeof(uint64_t)));
		*encoding_ptr = encoding_size;

		return encoding_size;
	}

	size_t decode(result& r, uint8_t* heap, uint64_t address) {
		if (address % 8 != 0) {
			r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
			return 0;
		}

		uint16_t *op_ptr = reinterpret_cast<uint16_t*>(heap + address + 1);
		op = static_cast<op_codes>(*op_ptr);

		uint8_t* encoding_ptr = heap + address;
		uint8_t encoding_size = *encoding_ptr;
		size = static_cast<op_sizes>(static_cast<uint8_t>(*(encoding_ptr + 3)));
		operands_count = static_cast<uint8_t>(*(encoding_ptr + 4));

		size_t offset = 5u;
		for (size_t i = 0u; i < operands_count ; ++i) {
			operands[i].type = static_cast<operand_types>(*(encoding_ptr + offset));
			++offset;

			operands[i].index = *(encoding_ptr + offset);
			++offset;

			switch (operands[i].type) {
				case operand_types::register_sp:
				case operand_types::register_pc:
				case operand_types::register_flags:
				case operand_types::register_status:
				case operand_types::register_integer:
				case operand_types::increment_register_pre:
				case operand_types::decrement_register_pre:
				case operand_types::increment_register_post:
				case operand_types::decrement_register_post:
				case operand_types::register_floating_point:
					break;
				case operand_types::increment_constant_pre:
				case operand_types::decrement_constant_pre:
				case operand_types::increment_constant_post:
				case operand_types::decrement_constant_post:
				case operand_types::constant_integer: {
					uint64_t *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
					operands[i].value.u64 = *constant_value_ptr;
					offset += sizeof(uint64_t);
					break;
				}
				case operand_types::constant_float: {
					double *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
					operands[i].value.d64 = *constant_value_ptr;
					offset += sizeof(double);
					break;
				}
			}
		}

		return encoding_size;
	}

	op_codes op = op_codes::nop;
	op_sizes size = op_sizes::none;
	uint8_t operands_count = 0;
	operand_encoding_t operands[4];
};
}

#endif //BOOTSTRAP__INSTRUCTION_H_
