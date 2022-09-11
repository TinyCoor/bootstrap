//
// Created by 12132 on 2022/3/26.
//
#include "ffi.h"
#include "terp.h"
#include "common/formatter.h"
#include "instruction.h"
#include "common/bytes.h"
#include "fmt/format.h"

namespace gfx {

terp::terp(size_t heap_size, size_t stack_size)
	: heap_size_(heap_size), stack_size_(stack_size), inst_cache_(this)
{
}

terp::~terp()
{
	if (heap_) {
		delete heap_;
		heap_ = nullptr;
		heap_size_ = 0;
	}
	free_heap_block_list();
}

bool terp::initialize(result& r)
{
	if (heap_ != nullptr) {
		return true;
	}

	call_vm_ = dcNewCallVM(4096);
	shared_libraries_.clear();
	heap_ = new uint8_t[heap_size_];
	heap_vector(heap_vectors_t::top_of_stack, heap_size_);
	heap_vector(heap_vectors_t::bottom_of_stack, heap_size_ - stack_size_);
	heap_vector(heap_vectors_t::program_start, program_start);
	reset();

	return !r.is_failed();
}

void terp::reset()
{
	registers_.pc = heap_vector(heap_vectors_t::program_start);
	registers_.sp = heap_vector(heap_vectors_t::top_of_stack);
	registers_.fr = 0;
	registers_.sr = 0;
	for (size_t i = 0; i < 64 ; ++i) {
		registers_.i[i] = 0;
		registers_.f[i] = 0.0;
	}
	inst_cache_.reset();
	dcReset(call_vm_);
	free_heap_block_list();
	exited_ = false;
}

void terp::dump_state(uint8_t count)
{
	fmt::print("-------------------------------------------------------------\n");
	fmt::print("PC =${:08x} | SP =${:08x} | FR =${:08x} | SR =${:08x}\n",
			   registers_.pc, registers_.sp, registers_.fr, registers_.sr);
	fmt::print("-------------------------------------------------------------\n");
	for (int index = 0; index < count; ++index) {
		fmt::print("I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x}\n",
			index, registers_.i[index],
			index + 1, registers_.i[index + 1],
			index + 2, registers_.i[index + 2],
			index + 3, registers_.i[index + 3]);
		index += 4;
	}
	fmt::print("-------------------------------------------------------------\n");
	for (int index = 0; index < count; ++index) {
		fmt::print("F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x}\n",
		   index, static_cast<uint64_t >(registers_.f[index]),
		   index + 1, static_cast<uint64_t >(registers_.f[index + 1]),
		   index + 2, static_cast<uint64_t >(registers_.f[index + 2]),
		   index + 3, static_cast<uint64_t >(registers_.f[index + 3]));
		index += 4;
	}
}

bool terp::step(result &r)
{
	instruction_t inst{};
	auto inst_size = inst_cache_.fetch(r, inst);
	if(inst_size == 0) {
		return false;
	}

	registers_.pc += inst_size;

	switch (inst.op) {
		case op_codes::nop:{
            break;
		}
		case op_codes::alloc: {
			uint64_t size;
			if (!get_operand_value(r, inst, 1, size)) {
				return false;
			}
			size *= op_size_in_bytes(inst.size);
			uint64_t address = alloc(size);
			if (!set_target_operand_value(r, inst, 0, address)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, address == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(address, inst.size));

		}break;
		case op_codes ::free: {
			uint64_t address;
			if (!get_operand_value(r, inst, 0, address)) {
				return false;
			}

			auto freed_size = free(address);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::negative, false);
			registers_.flags(register_file_t::flags_t::zero, freed_size != 0);

		}break;
		case op_codes::size: {
			uint64_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			uint64_t block_size = size(address);
			if (!set_target_operand_value(r, inst, 0, block_size)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, block_size == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(block_size, inst.size));

		} break;
		case op_codes::load: {
			uint64_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (inst.operands_count > 2) {
				uint64_t offset;
				if (!get_operand_value(r, inst, 2, offset)) {
					return false;
				}
				address += offset;
			}
			uint64_t value = *qword_ptr(address);
			if (!set_target_operand_value(r, inst, 0, value)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::swap:{
			uint64_t value;

			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}

			uint64_t result = 0;
			switch (inst.size) {
				case op_sizes::byte: {
					uint8_t byte_value = static_cast<uint8_t>(value);
					uint8_t upper_nybble = get_upper_nybble(byte_value);
					uint8_t lower_nybble = get_lower_nybble(byte_value);
					byte_value = set_upper_nybble(byte_value, lower_nybble);
					result = set_lower_nybble(byte_value, upper_nybble);
					break;
				}
				case op_sizes::word:
					result = endian_swap_word(static_cast<uint16_t>(value));
					break;
				case op_sizes::dword:
					result = endian_swap_dword(static_cast<uint32_t>(value));
					break;
				case op_sizes::qword:
				default:
					result = endian_swap_qword(value);
					break;
			}

			if (!set_target_operand_value(r, inst, 0, result))
				return false;

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::store: {
			uint64_t value;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			uint64_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (inst.operands_count > 2) {
				uint64_t offset;
				if (!get_operand_value(r, inst, 2, offset)) {
					return false;
				}
				address += offset;
			}
			*qword_ptr(address) = value;

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::inc: {
			uint8_t reg =  inst.operands[0].value.r8;
			uint64_t lhs_value = registers_.i[reg] ;
			uint64_t rhs_value = 1;
			uint64_t value = rhs_value + lhs_value;
			if (set_target_operand_value(r, inst, reg, value)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, value, inst.size));
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::carry, has_carry(value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::dec: {
			uint8_t reg = inst.operands[0].value.r8;
			uint64_t lhs_value = registers_.i[reg] ;
			uint64_t rhs_value = 1;
			uint64_t value = lhs_value - rhs_value;
			if (set_target_operand_value(r, inst, reg, value)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, value, inst.size));
			registers_.flags(register_file_t::flags_t::subtract, true);
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::carry, has_carry(value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));

		}break;
		case op_codes::copy: {
			uint64_t source_address, target_address;
			if (!get_operand_value(r, inst, 0, source_address)) {
				return false;
			}

			if (!get_operand_value(r, inst, 0, target_address)) {
				return false;
			}

			uint64_t  length ;
			if (!get_operand_value(r, inst, 2, length)) {
				return false;
			}

			memcpy(heap_ + target_address, heap_ + source_address, length * op_size_in_bytes(inst.size));
			registers_.flags(register_file_t::flags_t::zero, false);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::negative, false);
		}break;
		case op_codes::fill: {
			uint64_t value;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			uint64_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			uint64_t  length ;
			if (!get_operand_value(r, inst, 2, length)) {
				return false;
			}

			length *= op_size_in_bytes(inst.size);

			switch (inst.size) {
				case op_sizes::byte:
					memset(heap_ + address, static_cast<uint8_t>(value), length);
					break;
				case op_sizes::word:
					memset(heap_ + address, static_cast<uint16_t>(value), length);
					break;
				case op_sizes::dword:
					memset(heap_ + address, static_cast<uint32_t>(value), length);
					break;
				case op_sizes::qword:length *= sizeof(uint64_t);
					memset(heap_ + address, static_cast<uint64_t>(value), length);
					break;
				default:
					//// error
					break;
			}

			registers_.flags(register_file_t::flags_t::zero, false);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::negative, false);
		}break;
		case op_codes::move: {
			uint64_t source_value;
			if (!get_operand_value(r, inst, 0, source_value)) {
				return false;
			}
			if (!set_target_operand_value(r, inst, 1, source_value)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, source_value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(source_value, inst.size));
		}break;
		case op_codes::push: {
			uint64_t source_value;
			if (!get_operand_value(r, inst, 0, source_value)) {
				return false;
			}
			push(source_value);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, source_value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(source_value, inst.size));
		}break;
		case op_codes::pop:{
			uint64_t value = pop();
			if (!set_target_operand_value(r, inst, 0, value)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::dup: {
			uint64_t value = peek();
			push(value);

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::add:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t sum_result = lhs_value + rhs_value;
			if (!set_target_operand_value(r, inst, 0, sum_result )){
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, sum_result, inst.size));
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, sum_result == 0);
			registers_.flags(register_file_t::flags_t::carry, has_carry(sum_result, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(sum_result, inst.size));
		}break;
		case op_codes::sub:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t sub_res = lhs_value - rhs_value;
			if (!set_target_operand_value(r, inst, 0, sub_res)){
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, sub_res, inst.size));
			registers_.flags(register_file_t::flags_t::subtract, true);
			registers_.flags(register_file_t::flags_t::carry, rhs_value > lhs_value);
			registers_.flags(register_file_t::flags_t::zero, sub_res == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(sub_res, inst.size));
		}break;
		case op_codes::mul: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t product_res =  lhs_value * rhs_value;
			if (!set_target_operand_value(r, inst, 0,product_res)){
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, product_res, inst.size));
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, product_res == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(product_res, inst.size));
		}break;
		case op_codes::div:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value))
				return false;
			if (!get_operand_value(r, inst, 2, rhs_value))
				return false;
			uint64_t result = 0;
			if (rhs_value != 0) {
				result = lhs_value / rhs_value;
			}

			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, result, inst.size));
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::mod:{
			uint64_t lhs_value, rhs_value;

			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}

			uint64_t result = lhs_value % rhs_value;
			if (!set_target_operand_value(r, inst, 0,  result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, result, inst.size));
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::neg:{
			uint64_t value;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}
			int64_t negate_res = -static_cast<int64_t>(value);
			uint64_t result = static_cast<uint64_t>(negate_res);
			if (!set_target_operand_value(r, inst, 0, static_cast<uint64_t>(negate_res))) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::shl:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t result = lhs_value << rhs_value ;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::shr: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t result = lhs_value >> rhs_value;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::rol:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t left_rotated_value =rotl(lhs_value, static_cast<uint8_t>(rhs_value));
			if (!set_target_operand_value(r, inst, 0, left_rotated_value)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, left_rotated_value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(left_rotated_value, inst.size));
		}break;
		case op_codes::ror:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}

			uint64_t right_rotated_value = rotr(lhs_value, static_cast<uint8_t>(rhs_value));
			if (!set_target_operand_value(r, inst, 0, right_rotated_value)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, right_rotated_value == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(right_rotated_value, inst.size));
		}break;
		case op_codes::and_op:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t result = lhs_value & rhs_value;
			if (!set_target_operand_value(r, inst, 0, result )) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::or_op:{
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t result =  lhs_value | rhs_value;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::xor_op: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
			uint64_t result = lhs_value ^ rhs_value;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::not_op:{
			uint64_t value;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}

			uint64_t not_res = ~value;
			if (!set_target_operand_value(r, inst, 0, not_res)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, not_res == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(not_res, inst.size));
		}break;
		case op_codes::bis:{
			uint64_t value, bit_number;
			if (!get_operand_value(r, inst, 1, value))  {
				return false;
			}

			if (!get_operand_value(r, inst, 2, bit_number)) {
				return false;
			}

			uint64_t masked_value =static_cast<uint64_t > (1 << bit_number);
			uint64_t result = value | masked_value;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::zero, false);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::bic: {
			uint64_t value, bit_number;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, bit_number)) {
				return false;
			}

			uint64_t masked_value = static_cast<uint64_t>(~(1 << bit_number));
			uint64_t result =value & masked_value;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::zero, true);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::test:{
			uint64_t value, mask;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, mask)) {
				return false;
			}
			uint64_t result = value & mask;
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::cmp: {
			uint64_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 0, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, rhs_value)) {
				return false;
			}

			uint64_t result = lhs_value - rhs_value;
			registers_.flags(register_file_t::flags_t::overflow, has_overflow(lhs_value, rhs_value, result, inst.size));
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::subtract, true);
			registers_.flags(register_file_t::flags_t::carry, has_carry(result, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::bz: {
			uint64_t value, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (value == 0) {
				registers_.pc = address;
			}
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::carry, has_carry(value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::bnz: {
			uint64_t value, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (value != 0) {
				registers_.pc = address;
			}
			registers_.flags(register_file_t::flags_t::zero, value == 0);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::carry, has_carry(value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::tbz: {
			uint64_t value, mask, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, mask)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, address)) {
				return false;
			}

			uint64_t result = value & mask;
			if (result == 0) {
				registers_.pc = address;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		} break;
		case op_codes::tbnz: {
			uint64_t value, mask, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, mask)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, address)) {
				return false;
			}
			uint64_t result = value & mask;
			if (result != 0) {
				registers_.pc = address;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::bne: {
			uint64_t address;
			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::zero) == 0){
				registers_.pc = address;
			}
		}break;
		case op_codes::beq: {
			uint64_t address;
			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::zero)){
				registers_.pc = address;
			}
		} break;
		case op_codes::bg: {
			uint64_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (!registers_.flags(register_file_t::flags_t::carry)
				&& !registers_.flags(register_file_t::flags_t::zero)) {
				registers_.pc = address;
			}
		}break;
		case op_codes::bge: {
			uint64_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (!registers_.flags(register_file_t::flags_t::carry)) {
				registers_.pc = address;
			}
		}break;
		case op_codes::bl: {
			uint64_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::carry)
				|| registers_.flags(register_file_t::flags_t::zero)) {
				registers_.pc = address;
			}
		}break;
		case op_codes::ble:{
			uint64_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::carry)) {
				registers_.pc = address;
			}
		}break;
		case op_codes::jsr: {
			push(registers_.pc);
			uint64_t address;
			auto result = get_constant_address_or_pc_with_offset( r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			registers_.pc = address;
		}break;
		case op_codes::rts: {
			uint64_t address = pop();
			registers_.pc = address;
		}break;
		case op_codes::jmp: {
			uint64_t address;

			auto result = get_constant_address_or_pc_with_offset( r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}
			registers_.pc = address;
		}break;
		case op_codes::swi:{
			uint64_t index;
			if (!get_operand_value(r, inst, 0, index)) {
				return false;
			}

			size_t swi_offset = sizeof(uint64_t) * index;
			uint64_t swi_address = *qword_ptr(swi_offset);
			if (swi_address != 0) {
				/// TODO 恢复现场
				push(registers_.pc);
				registers_.pc = swi_address;
			}
		}break;
		case op_codes::trap:{
			uint64_t index;
			if (!get_operand_value(r, inst, 0, index)) {
				return false;
			}

			execute_trap(static_cast<uint8_t>(index));

		}break;
		case op_codes::ffi:{
			uint64_t address;
			if (!get_operand_value(r, inst, 0, address)) {
				return false;
			}

			auto it =foreign_functions_.find(reinterpret_cast<void*>(address));
			if (it == foreign_functions_.end()) {
				execute_trap(trap_invalid_ffi_call);
				break;
			}
			auto func_signature = &it->second;
			func_signature->apply_calling_convention(call_vm_);

			dcReset(call_vm_);

			std::vector<DCstruct* > structs_to_free{};
			for (auto& arg : func_signature->arguments) {
				arg.push(call_vm_, pop());
			}

			uint64_t result_value = func_signature->call(call_vm_, address);
			if (func_signature->return_value.type != ffi_types_t::void_type) {
				push(result_value);
			}
		}break;
		case op_codes::meta: {
			uint64_t meta_data_size;

			if (!get_operand_value(r, inst, 0, meta_data_size)) {
				return false;
			}

		}break;
		case op_codes::exit: {
			exited_ = true;
		}break;
        case op_codes::setz: {
            uint64_t result = registers_.flags(register_file_t::flags_t::zero) ? 1 : 0;
            if (!set_target_operand_value(r, inst, 0, result)) {
                return false;
            }

            break;
        }
        case op_codes::setnz: {
            uint64_t result = !registers_.flags(register_file_t::flags_t::zero) ? 1 : 0;
            if (!set_target_operand_value(r, inst, 0, result)) {
                return false;
            }
            break;
        }
		default:
			break;
	}

	return !r.is_failed();
}

uint64_t terp::pop()
{
	uint64_t value = *qword_ptr(registers_.sp);
	registers_.sp += sizeof(uint64_t);
	return value;
}

void terp::push(uint64_t value)
{
	registers_.sp -= sizeof(uint64_t);
	*qword_ptr(registers_.sp) = value;
}

void terp::dump_heap(uint64_t offset, size_t size)
{
	auto pMemory = formatter::Hex((void*)(heap_ + offset), size);
	fmt::print("{}\n",pMemory);
}

bool terp::get_operand_value(result& r, const instruction_t& instruction, uint8_t operand_index, uint64_t& value) const
{
	auto& operand = instruction.operands[operand_index];

	if (operand.is_reg()) {
		if (operand.is_integer()) {
			auto reg = static_cast<i_registers_t>(operand.value.r8);
			switch (reg) {
				case i_registers_t::pc: {
					value = registers_.pc;
					break;
				}
				case i_registers_t::sp: {
					value = registers_.sp;
					break;
				}
                case i_registers_t::fp: {
                    value = registers_.fp;
                    break;
                }
				case i_registers_t::fr: {
					value = registers_.fr;
					break;
				}
				case i_registers_t::sr: {
					value = registers_.sr;
					break;
				}
				default: {
					value = registers_.i[reg];
					break;
				}
			}
		} else {
			value = static_cast<uint64_t>(registers_.f[operand.value.r8]);
		}
	} else {
		if (operand.is_integer()) {
			value = operand.value.u64;
		} else {
			value = static_cast<uint64_t>(operand.value.d64);
		}
	}

	return true;
}

bool terp::get_operand_value(result& r, const instruction_t& instruction, uint8_t operand_index, double& value) const
{
	auto& operand = instruction.operands[operand_index];
	if (operand.is_reg()) {
		if (operand.is_integer()) {
			value = registers_.i[operand.value.r8];
		} else {
			value = registers_.f[operand.value.r8];
		}
	} else {
		if (operand.is_integer()) {
			value = operand.value.u64;
		} else {
			value = operand.value.d64;
		}
	}
	return true;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, uint64_t value)
{
	auto& operand = inst.operands[operand_index];

	if (operand.is_reg()) {
		if (operand.is_integer()) {
			auto reg = static_cast<i_registers_t>(operand.value.r8);
			switch (reg) {
				case i_registers_t::pc: {
					registers_.pc = set_zoned_value(registers_.pc, value, inst.size);
					break;
				}
				case i_registers_t::sp: {
					registers_.sp = set_zoned_value(registers_.sp, value, inst.size);
					break;
				}
                case i_registers_t::fp: {
                    registers_.fp = set_zoned_value(registers_.fp, value, inst.size);
                    break;
                }
				case i_registers_t::fr: {
					registers_.fr =  set_zoned_value(registers_.fr, value, inst.size);
					break;
				}
				case i_registers_t::sr: {
					registers_.sr =  set_zoned_value(registers_.sr, value, inst.size);
					break;
				}
				default: {
					registers_.i[reg] = value;
					registers_.flags(register_file_t::flags_t::zero, value == 0);
					break;
				}
			}
		} else {
			registers_.f[operand.value.r8] = value;
		}
	} else {
		r.add_message("B006", "constant cannot be a target operand type.", true);
		return false;
	}

	return true;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index, double value)
{
	auto& operand = inst.operands[operand_index];

	if (operand.is_reg()) {
		if (operand.is_integer()) {
			auto integer_value = static_cast<uint64_t>(value);
			auto reg = static_cast<i_registers_t>(operand.value.r8);
			switch (reg) {
				case i_registers_t::pc: {
					registers_.pc = set_zoned_value(registers_.pc,integer_value, inst.size);
					break;
				}
				case i_registers_t::sp: {
					registers_.sp =set_zoned_value(registers_.sp,integer_value, inst.size);
					break;
				}
                case i_registers_t::fp: {
                    registers_.fp = set_zoned_value(registers_.fp, integer_value, inst.size);
                    break;
                }
				case i_registers_t::fr: {
					registers_.fr = set_zoned_value(registers_.fr, integer_value, inst.size);
					break;
				}
				case i_registers_t::sr: {
					registers_.sr = set_zoned_value(registers_.sr, integer_value, inst.size);
					break;
				}
				default: {
					registers_.i[reg] = set_zoned_value(registers_.i[reg], integer_value,inst.size);
					break;
				}
			}
		} else {
			registers_.f[operand.value.r8] = value;
		}
	} else {
		r.add_message("B006", "constant cannot be a target operand type.", true);
		return false;
	}

	return true;
}

std::string terp::disassemble(result &r, uint64_t address)
{
	std::stringstream stream;
	while (true) {
		instruction_t inst{};

		auto inst_size = inst_cache_.fetch_at(r, inst, address);
		if (inst_size == 0) {
			break;
		}
		stream << fmt::format("${:016X}: ", address)
			   << inst.disassemble()
			   << fmt::format(" (${:02X} bytes)\n", inst_size);

		if (inst.op == op_codes::exit) {
			break;
		}
		address += inst_size;
	}
	return stream.str();
}

void terp::register_trap(uint8_t index, const terp::trap_callable &callable)
{
	traps_.insert(std::make_pair(index, callable));
}

void terp::remove_trap(uint8_t index)
{
	traps_.erase(index);
}

uint64_t terp::peek() const
{
	uint64_t value = *qword_ptr(registers_.sp);
	return value;
}

void terp::swi(uint8_t index, uint64_t address)
{
	size_t swi_address = interrupt_vector_table_start + (sizeof(uint64_t) * index);
	*qword_ptr(swi_address) = address;
}

std::vector<uint64_t> terp::jump_to_subroutine(result &r, uint64_t address)
{
	std::vector<uint64_t> return_values;

	auto return_address = registers_.pc;
	push(return_address);
	registers_.pc = address;

	while (!has_exited()) {
		// XXX: need to introduce a terp_step_result_t
		auto result = step(r);
		// XXX: did an RTS just execute?
		//      does _registers.pc == return_address?  if so, we're done
		if (!result) {
			break;
		}
	}

	// XXX: how do we handle multiple return values?
	// XXX: pull return values from the stack
	return return_values;
}
uint64_t terp::heap_vector(heap_vectors_t vector) const
{
	size_t heap_vector_address = heap_vector_table_start
		+ (sizeof(uint64_t) * static_cast<uint8_t>(vector));
	return *qword_ptr(heap_vector_address);
}

void terp::heap_vector(heap_vectors_t vector, uint64_t address)
{
	size_t heap_vector_address = heap_vector_table_start
		+ (sizeof(uint64_t) * static_cast<uint8_t>(vector));
	*qword_ptr(heap_vector_address) = address;
}

uint64_t terp::set_zoned_value(uint64_t source, uint64_t value, op_sizes size)
{
	uint64_t result = source;
	switch (size) {
		case op_sizes::byte: {
			result &= mask_byte_clear;
			result |= (value & mask_byte);
			break;
		}
		case op_sizes::word: {
			result &= mask_word_clear;
			result |= (value & mask_word);
			break;
		}
		case op_sizes::dword: {
			result &= mask_dword_clear;
			result |= (value & mask_dword);
			break;
		}
		case op_sizes::qword:
		default:
			result = value;
			break;
	}
	return result;
}

bool terp::has_overflow(uint64_t lhs, uint64_t rhs, uint64_t result, op_sizes size)
{
	switch (size) {
		case op_sizes::byte:
			return ((~(lhs ^ rhs)) & (lhs ^ result) & mask_byte_negative) != 0;
		case op_sizes::word:
			return ((~(lhs ^ rhs)) & (lhs ^ result) & mask_word_negative) != 0;
		case op_sizes::dword:
			return ((~(lhs ^ rhs)) & (lhs ^ result) & mask_dword_negative) != 0;
		case op_sizes::qword:
		default: {
			return ((~(lhs ^ rhs)) & (lhs ^ result) & mask_qword_negative) != 0;
		}
	}
	return false;
}

bool terp::is_negative(uint64_t value, op_sizes size)
{
	switch (size) {
		case op_sizes::byte: {
			return (value & mask_byte_negative) != 0;
		}
		case op_sizes::word: {
			return (value & mask_word_negative) != 0;
		}
		case op_sizes::dword: {
			return (value & mask_dword_negative) != 0;
		}
		case op_sizes::qword:
		default:
			return (value & mask_qword_negative) != 0;
	}
	return false;
}

bool terp::has_carry(uint64_t value, op_sizes size)
{
	switch (size) {
		case op_sizes::byte:
			return value > UINT8_MAX;
		case op_sizes::word:
			return value > UINT16_MAX;
		case op_sizes::dword:
			return value > UINT32_MAX;
		case op_sizes::qword:
		default:
			return value > UINT64_MAX;
	}
}

bool terp::get_constant_address_or_pc_with_offset(result &r, const instruction_t &inst, uint8_t operand_index,
	uint64_t inst_size, uint64_t &address)
{
	if (!get_operand_value(r, inst, operand_index, address)) {
		return false;
	}

	if (inst.operands_count >= 2) {
		uint64_t offset;

		uint8_t offset_index = static_cast<uint8_t>(operand_index + 1);
		if (!get_operand_value(r, inst, offset_index, offset)) {
			return false;
		}

		if (inst.operands[offset_index].is_negative()) {
			address -= offset + inst_size;
		} else {
			address += offset - inst_size;
		}
	}

	return true;
}

class shared_library* terp::load_shared_library(result &r, const std::filesystem::path &path)
{
	auto it = shared_libraries_.find(path.string());
	if (it != shared_libraries_.end()) {
		return &it->second;
	}

	class shared_library shared_library {};
	if (!shared_library.initialize(r, path)) {
		return nullptr;
	}
	auto pair = shared_libraries_.insert(std::make_pair(path.string(), shared_library));
	return &(*pair.first).second;
}

uint64_t terp::alloc(uint64_t size)
{
	uint64_t size_delta = size;
	heap_block_t* best_sized_block = nullptr;
	auto current_block = head_heap_block_;

	while (current_block != nullptr) {
		if (current_block->is_free()) {
			if (current_block->size == size) {
				current_block->mark_allocated();
				return current_block->address;
			} else if (current_block->size > size) {
				auto local_size_delta = current_block->size - size;
				if (best_sized_block == nullptr
					||  local_size_delta < size_delta) {
					size_delta = local_size_delta;
					best_sized_block = current_block;
				}
			}
		}
		current_block = current_block->next;
	}

	if (best_sized_block != nullptr) {
		// if the block is over-sized by 64 bytes or less, just use it as-is
		if (size_delta <= 64) {
			best_sized_block->mark_allocated();
			return best_sized_block->address;
		} else {
			// otherwise, we need to split the block in two
			auto new_block = new heap_block_t;
			new_block->size = size;
			new_block->mark_allocated();
			new_block->prev = best_sized_block->prev;
			if (new_block->prev != nullptr) {
				new_block->prev->next = new_block;
			}

			new_block->next = best_sized_block;
			new_block->address = best_sized_block->address;

			best_sized_block->prev = new_block;
			best_sized_block->address += size;
			best_sized_block->size -= size;

			if (new_block->prev == nullptr) {
				head_heap_block_ = new_block;
			}

			address_blocks_[new_block->address] = new_block;
			address_blocks_[best_sized_block->address] = best_sized_block;

			return best_sized_block->prev->address;
		}
	}

	return 0;
}

uint64_t terp::free(uint64_t address)
{
	auto it = address_blocks_.find(address);
	if (it == address_blocks_.end()) {
		return 0;
	}

	heap_block_t* freed_block = it->second;
	auto freed_size = freed_block->size;
	freed_block->clear_allocated();

	// coalesce free blocks
	// first, we walk down the prev chain until we find a non-free block
	// then, we walk down the next chain until we find a non-free block
	// because blocks are known to be adjacent to each other in the heap,
	//          we then coalesce these blocks into one

	std::vector<heap_block_t*> delete_list {};
	uint64_t new_size = 0;

	auto first_free_block = freed_block;
	while (true) {
		auto prev = first_free_block->prev;
		if (prev == nullptr || !prev->is_free()) {
			break;
		}

		first_free_block = prev;
	}

	auto last_free_block = freed_block;
	while (true) {
		auto next = last_free_block->next;
		if (next == nullptr || !next->is_free()) {
			break;
		}

		last_free_block = next;
	}

	auto current_node = first_free_block;
	while (true) {
		delete_list.emplace_back(current_node);
		new_size += current_node->size;

		if (current_node == last_free_block) {
			break;
		}
		current_node = current_node->next;
	}

	if (first_free_block != last_free_block) {
		auto new_block = new heap_block_t;
		new_block->size = new_size;

		new_block->next = last_free_block->next;
		if (new_block->next != nullptr) {
			new_block->next->prev = new_block;
		}

		new_block->prev = first_free_block->prev;
		if (new_block->prev != nullptr) {
			new_block->prev->next = new_block;
		}


		new_block->address = first_free_block->address;

		for (auto block : delete_list) {
			address_blocks_.erase(block->address);
			delete block;
		}

		if (new_block->prev == nullptr) {
			head_heap_block_ = new_block;
		}

		address_blocks_[new_block->address] = new_block;
	}

	return freed_size;
}
uint64_t terp::size(uint64_t address)
{
	auto it = address_blocks_.find(address);
	if (it == address_blocks_.end())
		return 0;
	return it->second->size;
}

void terp::heap_free_space_begin(uint64_t address)
{
	heap_vector(heap_vectors_t::free_space_start, address);
	head_heap_block_ = new heap_block_t;
	head_heap_block_->address = address;
	head_heap_block_->size = heap_vector(heap_vectors_t::bottom_of_stack) - address;
	address_blocks_.insert(std::make_pair(head_heap_block_->address, head_heap_block_));

}
void terp::free_heap_block_list()
{
	address_blocks_.clear();

	if (head_heap_block_ == nullptr) {
		return;
	}

	auto current_block = head_heap_block_;
	while (current_block != nullptr) {
		auto next_block = current_block->next;
		delete current_block;
		current_block = next_block;
	}

	head_heap_block_ = nullptr;
}

void terp::execute_trap(uint8_t index)
{
	auto it = traps_.find(static_cast<uint8_t>(index));
	if (it== traps_.end()) {
		return;
	}
	it->second(this);
}

bool terp::register_foreign_function(result& r, function_signature_t &signature)
{
	if (signature.func_ptr !=nullptr) {
		auto it = foreign_functions_.find(signature.func_ptr);
		if (it != foreign_functions_.end()) {
			return true;
		}
	}
	if (signature.library == nullptr) {
		/// TODO error msg
		return false;
	}

	auto func_ptr =signature.library->symbol_address(signature.symbol);
	if (func_ptr == nullptr) {
		/// TODO error msg
		return false;
	}

	signature.func_ptr= func_ptr;
	foreign_functions_.insert(std::make_pair(func_ptr,signature));

	return true;
}

shared_library *terp::shared_library(const std::filesystem::path &path)
{
	auto it = shared_libraries_.find(path.string());
	if (it == shared_libraries_.end()) {
		return nullptr;
	}
	return &it->second;
}

void terp::dump_shared_libraries()
{
	fmt::print("\n{:32}{:64}{:17}\n", "Image Name", "Symbol Name", "Address");
	fmt::print("{}\n", std::string(120, '-'));
	for (const auto& kvp : shared_libraries_) {
		auto index = 0;
		for (const auto& entry : kvp.second.symbols()) {
			fmt::print("{:32}{:64}${:016X}\n",
				index == 0 ? kvp.first.substr(0, std::min<size_t>(32, kvp.first.length())) : "",
				entry.first.substr(0, std::min<size_t>(64, entry.first.length())),
				reinterpret_cast<uint64_t>(entry.second));
			++index;
		}
	}
	fmt::print("\n");
}

}