//
// Created by 12132 on 2022/4/3.
//
#include "instruction.h"
namespace gfx{
size_t instruction_t::encode(result& r, uint8_t* heap, uint64_t address)
{
	if (address % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}
	uint8_t encoding_size = INSTRUCTION_PRE_LENGTH;
	size_t offset = INSTRUCTION_PRE_LENGTH;

	auto encoding_ptr = heap + address;
	*(encoding_ptr + 1) = static_cast<uint8_t>(op);
	*(encoding_ptr + 2) = static_cast<uint8_t>(size);
	*(encoding_ptr + 3) = operands_count;

	for (size_t i = 0u; i < operands_count ; ++i) {
		*(encoding_ptr + offset) = static_cast<uint8_t>(operands[i].type);
		++offset;

		*(encoding_ptr + offset) = operands[i].index;
		++offset;
		encoding_size += 2u;
		switch (operands[i].type) {
			case operand_types::increment_constant_pre:
			case operand_types::increment_constant_post:
			case operand_types::decrement_constant_pre:
			case operand_types::decrement_constant_post:
			case operand_types::constant_offset_negative:
			case operand_types::constant_offset_positive:
			case operand_types::constant_integer: {
				switch (size) {
					case op_sizes::byte: {
						uint8_t *constant_value_ptr = reinterpret_cast<uint8_t *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<uint8_t>(operands[i].value.u64);
						offset += sizeof(uint8_t);
						encoding_size += sizeof(uint8_t);
						break;
					}
					case op_sizes::word:{
						uint16_t *constant_value_ptr = reinterpret_cast<uint16_t *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<uint16_t>(operands[i].value.u64);
						offset += sizeof(uint16_t);
						encoding_size += sizeof(uint16_t);
						break;
					}
					case op_sizes::dword: {
						uint32_t *constant_value_ptr = reinterpret_cast<uint32_t *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<uint32_t>(operands[i].value.u64);
						offset += sizeof(uint32_t);
						encoding_size += sizeof(uint32_t);
						break;
					}
					case op_sizes::qword: {
						uint64_t *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<uint64_t>(operands[i].value.u64);
						offset += sizeof(uint64_t);
						encoding_size += sizeof(uint64_t);
						break;
					}
					case op_sizes::none:{
						r.add_message("B009", "constant integer can no have a size of none ", true);
						break;
					}
				}
				break;
			}
			case operand_types::constant_float: {
				switch (size) {
					case op_sizes::dword: {
						auto *constant_value_ptr = reinterpret_cast<float *>(encoding_ptr + offset);
						*constant_value_ptr = static_cast<float >(operands[i].value.d64);
						offset += sizeof(float);
						encoding_size += sizeof(float);
					}break;

					case op_sizes::qword:{
						auto *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
						*constant_value_ptr = operands[i].value.d64;
						offset += sizeof(double);
						encoding_size += sizeof(double);
					}break;
					case op_sizes::byte:
					case op_sizes::word:
					case op_sizes::none:{
						r.add_message("B009", "constant float can no have a size of none byte word ", true);
					}
				}
				break;
			}
			default:{
				break;
			}
		}
	}

	encoding_size = static_cast<uint8_t>(align(encoding_size, sizeof(uint64_t)));
	*encoding_ptr = encoding_size;

	return encoding_size;
}

size_t instruction_t::decode(result& r, uint8_t* heap, uint64_t address)
{
	if (address % 8 != 0) {
		r.add_message("B003", "Instruction must encoded on 8-byte boundaries.", true);
		return 0;
	}

	uint8_t* encoding_ptr = heap + address;
	uint8_t encoding_size = *encoding_ptr;
	op = static_cast<op_codes>(*(encoding_ptr +1));
	size = static_cast<op_sizes>(static_cast<uint8_t>(*(encoding_ptr + 2)));
	operands_count = static_cast<uint8_t>(*(encoding_ptr + 3));

	size_t offset = INSTRUCTION_PRE_LENGTH;
	for (size_t i = 0u; i < operands_count ; ++i) {
		operands[i].type = static_cast<operand_types>(*(encoding_ptr + offset));
		++offset;

		operands[i].index = *(encoding_ptr + offset);
		++offset;

		switch (operands[i].type) {
			case operand_types::increment_constant_pre:
			case operand_types::decrement_constant_pre:
			case operand_types::increment_constant_post:
			case operand_types::decrement_constant_post:
			case operand_types::constant_offset_positive:
			case operand_types::constant_offset_negative:
			case operand_types::constant_integer: {
				switch (size) {
					case op_sizes::byte: {
						auto *constant_value_ptr = reinterpret_cast<uint8_t *>(encoding_ptr + offset);
						operands[i].value.u64 = *constant_value_ptr;
						offset += sizeof(uint8_t);
					}break;
					case op_sizes::word: {
						auto *constant_value_ptr = reinterpret_cast<uint16_t *>(encoding_ptr + offset);
						operands[i].value.u64 = *constant_value_ptr;
						offset += sizeof(uint16_t);
					}break;
					case op_sizes::dword: {
						auto *constant_value_ptr = reinterpret_cast<uint32_t *>(encoding_ptr + offset);
						operands[i].value.u64 = *constant_value_ptr;
						offset += sizeof(uint32_t);
					}break;
					case op_sizes::qword: {
						auto *constant_value_ptr = reinterpret_cast<uint64_t *>(encoding_ptr + offset);
						operands[i].value.u64 = *constant_value_ptr;
						offset += sizeof(uint64_t);
					}break;
					case op_sizes::none: {
						r.add_message("B009", "constant integer can no have a size of none ", true);
						break;
					}
				}
			}break;
			case operand_types::constant_float: {
				switch (size) {
					case op_sizes::dword: {
						auto *constant_value_ptr = reinterpret_cast<float *>(encoding_ptr + offset);
						operands[i].value.d64 = *constant_value_ptr;
						offset += sizeof(float);
					}break;
					case op_sizes::qword: {
						auto *constant_value_ptr = reinterpret_cast<double *>(encoding_ptr + offset);
						operands[i].value.d64 = *constant_value_ptr;
						offset += sizeof(double);
					}break;
					case op_sizes::byte:
					case op_sizes::word:
					case op_sizes::none: {
						r.add_message("B010", "constant float can no have a size of none byte word ", true);
						break;
					}
				}
			}
			default:{
				break;
			}
		}
	}

	return encoding_size;
}

size_t instruction_t::align(uint64_t value, size_t size) const
{
	auto offset = value % size;
	return offset ? value + (size - offset) : value;
}

size_t instruction_t::encoding_size() const
{
	size_t encoding_size = INSTRUCTION_PRE_LENGTH;

	for (size_t i = 0; i < operands_count; i++) {
		encoding_size += 2;
		switch (operands[i].type) {
			case operand_types::increment_constant_pre:
			case operand_types::increment_constant_post:
			case operand_types::decrement_constant_pre:
			case operand_types::decrement_constant_post:
			case operand_types::constant_offset_negative:
			case operand_types::constant_offset_positive:
			case operand_types::constant_integer: {
				switch (size) {
					case op_sizes::byte:{
						encoding_size += sizeof(uint8_t);
						break;
					}
					case op_sizes::word:{
						encoding_size += sizeof(uint16_t);
						break;
					}
					case op_sizes::dword:{
						encoding_size += sizeof(uint32_t);
						break;
					}
					case op_sizes::qword:{
						encoding_size += sizeof(uint64_t);
					}break;
					case op_sizes::none:{
						break;
					}
				}
				break;
			}
			case operand_types::constant_float: {
				switch (size) {
					case op_sizes::dword:{
						encoding_size += sizeof(float);
						break;
					}
					case op_sizes::qword:{
						encoding_size += sizeof(double);
						break;
					}
					default:
						break;
				}
			}
			default:
				break;
		}
	}

	encoding_size = static_cast<uint8_t>(align(encoding_size, sizeof(uint64_t)));

	return encoding_size;
}

void instruction_t::patch_branch_address(uint64_t address)
{
	operands[0].value.u64 = align(address, sizeof(uint64_t));
}
}