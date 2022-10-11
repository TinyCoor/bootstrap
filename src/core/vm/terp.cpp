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
	registers_.r[register_pc].qw = heap_vector(heap_vectors_t::program_start);
	registers_.r[register_sp].qw = heap_vector(heap_vectors_t::top_of_stack);
	registers_.r[register_fr].qw = 0;
	registers_.r[register_sr].qw = 0;
	for (size_t i = 0; i < 64 ; ++i) {
		registers_.r[i].qw = 0;
	}
	inst_cache_.reset();
	dcReset(call_vm_);
	free_heap_block_list();
	exited_ = false;
}

bool terp::run(result &r)
{
    while (!has_exited()) {
        if (!step(r)) {
            return false;
        }
    }
    return true;
}

void terp::dump_state(uint8_t count)
{
	fmt::print("-------------------------------------------------------------\n");
	fmt::print("PC =${:08x} | SP =${:08x} | FR =${:08x} | SR =${:08x}\n",
			   registers_.r[register_pc].qw, registers_.r[register_sp].qw, registers_.r[register_fr].qw,
               registers_.r[register_sr].qw);
	fmt::print("-------------------------------------------------------------\n");
    uint8_t index = register_integer_start;
	for (int x = 0; x < count; ++x) {
		fmt::print("I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x} | I{:02}=${:08x}\n",
			index, registers_.r[index].qw, index + 1, registers_.r[index + 1].qw,
			index + 2, registers_.r[index + 2].qw, index + 3, registers_.r[index + 3].qw);
		index += 4;
	}
	fmt::print("-------------------------------------------------------------\n");
    index = register_float_start;
	for (int x = 0; x < count; ++x) {
		fmt::print("F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x} | F{:02}=${:08x}\n",
		   index, static_cast<uint64_t >(registers_.r[index].qw),
		   index + 1, static_cast<uint64_t >(registers_.r[index + 1].qw),
		   index + 2, static_cast<uint64_t >(registers_.r[index + 2].qw),
		   index + 3, static_cast<uint64_t >(registers_.r[index + 3].qw));
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

	registers_.r[register_pc].qw += inst_size;

	switch (inst.op) {
		case op_codes::nop:{
            break;
		}
		case op_codes::alloc: {
            operand_value_t size;
			if (!get_operand_value(r, inst, 1, size)) {
				return false;
			}
			size.alias.u *= op_size_in_bytes(inst.size);
            operand_value_t  address;
            address.alias.u = alloc(size.alias.u);
            if (address.alias.u == 0) {
                execute_trap(trap_out_of_memory);
                return false;
            }
			if (!set_target_operand_value(r, inst, 0, address)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, address.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(address, inst.size));

		}break;
		case op_codes ::free: {
			operand_value_t address;
			if (!get_operand_value(r, inst, 0, address)) {
				return false;
			}

			auto freed_size = free(address.alias.u);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::negative, false);
			registers_.flags(register_file_t::flags_t::zero, freed_size != 0);
            break;
		}
		case op_codes::size: {
			operand_value_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}
            operand_value_t block_size;
			block_size.alias.u = size(address.alias.u);
			if (!set_target_operand_value(r, inst, 0, block_size)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, block_size.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(block_size, inst.size));
            break;
		}
		case op_codes::load: {
			operand_value_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (inst.operands_count > 2) {
				operand_value_t offset;
				if (!get_operand_value(r, inst, 2, offset)) {
					return false;
				}
				address.alias.u += offset.alias.u;
			}

            operand_value_t loaded_data;
			loaded_data.alias.u = read(inst.size, address.alias.u);
			if (!set_target_operand_value(r, inst, 0, loaded_data)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, loaded_data.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(loaded_data, inst.size));
            break;
		}
		case op_codes::swap:{
			operand_value_t value;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = 0;
			switch (inst.size) {
				case op_sizes::byte: {
					uint8_t byte_value = static_cast<uint8_t>(value.alias.u);
					uint8_t upper_nybble = get_upper_nybble(byte_value);
					uint8_t lower_nybble = get_lower_nybble(byte_value);
					byte_value = set_upper_nybble(byte_value, lower_nybble);
					result.alias.u = set_lower_nybble(byte_value, upper_nybble);
					break;
				}
				case op_sizes::word:
					result.alias.u = endian_swap_word(static_cast<uint16_t>(value.alias.u));
					break;
				case op_sizes::dword:
					result.alias.u = endian_swap_dword(static_cast<uint32_t>(value.alias.u));
					break;
				case op_sizes::qword:
				default:
					result.alias.u = endian_swap_qword(value.alias.u);
					break;
			}

			if (!set_target_operand_value(r, inst, 0, result)) {
                return false;
            }

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
            break;
		}
		case op_codes::store: {
            operand_value_t address;
			if (!get_operand_value(r, inst, 0, address)) {
				return false;
			}

            operand_value_t value;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}

			if (inst.operands_count > 2) {
				operand_value_t offset;
				if (!get_operand_value(r, inst, 2, offset)) {
					return false;
				}
				address.alias.u += offset.alias.u;
			}
            write(inst.size, address.alias.u, value.alias.u);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::inc: {
            operand_value_t reg_value;
            if (get_operand_value(r, inst, 0, reg_value)) {
                return false;
            }
	        operand_value_t new_value;
            if (reg_value.type == register_type_t::floating_point) {
                new_value.alias.d = reg_value.alias.d + 1.0;
            } else {
                new_value.alias.u = reg_value.alias.u + 1;
            }
			if (set_target_operand_value(r, inst, 0, reg_value)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
                has_overflow(reg_value.alias.u, 1, new_value.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::zero, new_value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::carry, has_carry(new_value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(new_value, inst.size));
		}break;
		case op_codes::dec: {
            operand_value_t reg_value;
            if (get_operand_value(r, inst, 0, reg_value)) {
                return false;
            }
            operand_value_t new_value;
            if (reg_value.type == register_type_t::floating_point) {
                new_value.alias.d = reg_value.alias.d - 1.0;
            } else {
                new_value.alias.u = reg_value.alias.u - 1;
            }
			if (set_target_operand_value(r, inst, 0, reg_value)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
                has_overflow(reg_value.alias.u, 1, new_value.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::subtract, true);
			registers_.flags(register_file_t::flags_t::zero, new_value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::carry, has_carry(new_value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(new_value, inst.size));

		}break;
		case op_codes::copy: {
			operand_value_t source_address, target_address;
			if (!get_operand_value(r, inst, 0, source_address)) {
				return false;
			}

			if (!get_operand_value(r, inst, 0, target_address)) {
				return false;
			}

			operand_value_t length ;
			if (!get_operand_value(r, inst, 2, length)) {
				return false;
			}

			memcpy(heap_ + target_address.alias.u, heap_ + source_address.alias.u, length.alias.u * op_size_in_bytes(inst.size));
			registers_.flags(register_file_t::flags_t::zero, false);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::negative, false);
		}break;
		case op_codes::fill: {
			operand_value_t value;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			operand_value_t address;
			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			operand_value_t length;
			if (!get_operand_value(r, inst, 2, length)) {
				return false;
			}

			length.alias.u *= op_size_in_bytes(inst.size);

			switch (inst.size) {
				case op_sizes::byte:
					memset(heap_ + address.alias.u, static_cast<uint8_t>(value.alias.u), length.alias.u);
					break;
				case op_sizes::word:
					memset(heap_ + address.alias.u, static_cast<uint16_t>(value.alias.u), length.alias.u);
					break;
				case op_sizes::dword:
					memset(heap_ + address.alias.u, static_cast<uint32_t>(value.alias.u), length.alias.u);
					break;
				case op_sizes::qword:
                    length.alias.u *= sizeof(uint64_t);
					memset(heap_ + address.alias.u, static_cast<uint64_t>(value.alias.u), length.alias.u);
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
			operand_value_t source_value;
            operand_value_t offset;

            if (inst.operands_count > 2) {
                if (!get_operand_value(r, inst, 2, offset)) {
                    return false;
                }
            }
			if (!get_operand_value(r, inst, 1, source_value)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = source_value.alias.u + offset.alias.u;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, source_value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(source_value, inst.size));
		}break;
		case op_codes::push: {
			operand_value_t source_value;
			if (!get_operand_value(r, inst, 0, source_value)) {
				return false;
			}
			push(source_value.alias.u);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, source_value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(source_value, inst.size));
		}break;
		case op_codes::pop:{
            operand_value_t value;
            value.alias.u = pop();
			if (!set_target_operand_value(r, inst, 0, value)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::zero, value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::dup: {
			operand_value_t value;
            value.alias.u = peek();
			push(value.alias.u);

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;

		case op_codes::add:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t sum_result;
            if (lhs_value.type == register_type_t::floating_point
                &&  rhs_value.type == register_type_t::floating_point) {
                sum_result.alias.d = lhs_value.alias.d + rhs_value.alias.d;
            } else {
                sum_result.alias.u = lhs_value.alias.u + rhs_value.alias.u;
            }
			if (!set_target_operand_value(r, inst, 0, sum_result)){
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
                has_overflow(lhs_value.alias.u, rhs_value.alias.u, sum_result.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, sum_result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::carry, has_carry(sum_result, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(sum_result, inst.size));
		}break;
		case op_codes::sub:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t sub_res;
            if (lhs_value.type == register_type_t::floating_point
                &&  rhs_value.type == register_type_t::floating_point) {
                sub_res.alias.d = lhs_value.alias.d - rhs_value.alias.d;
            } else {
                sub_res.alias.u = lhs_value.alias.u - rhs_value.alias.u;
            }
			if (!set_target_operand_value(r, inst, 0, sub_res)){
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
               has_overflow(lhs_value.alias.u, rhs_value.alias.u, sub_res.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::subtract, true);
			registers_.flags(register_file_t::flags_t::carry, rhs_value.alias.u > lhs_value.alias.u);
			registers_.flags(register_file_t::flags_t::zero, sub_res.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(sub_res, inst.size));
		}break;
		case op_codes::mul: {
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t product_res;
            if (lhs_value.type == register_type_t::floating_point
                &&  rhs_value.type == register_type_t::floating_point) {
                product_res.alias.d = lhs_value.alias.d * rhs_value.alias.d;
            } else {
                product_res.alias.u = lhs_value.alias.u * rhs_value.alias.u;
            }
			if (!set_target_operand_value(r, inst, 0,product_res)){
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
                has_overflow(lhs_value.alias.u, rhs_value.alias.u, product_res.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, product_res.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(product_res, inst.size));
            break;
		}
		case op_codes::div:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
                return false;
            }

			if (!get_operand_value(r, inst, 2, rhs_value)) {
                return false;
            }
            operand_value_t result;
            if (lhs_value.type == register_type_t::floating_point
                &&  rhs_value.type == register_type_t::floating_point) {
                if (rhs_value.alias.d != 0) {
                    result.alias.d = lhs_value.alias.d / rhs_value.alias.d;
                }
            } else {
                if (rhs_value.alias.u != 0) {
                    result.alias.u = lhs_value.alias.u / rhs_value.alias.u;
                }
            }
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
                has_overflow(lhs_value.alias.u, rhs_value.alias.u, result.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::mod:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t result;
			result.alias.u = lhs_value.alias.u % rhs_value.alias.u;
			if (!set_target_operand_value(r, inst, 0,  result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::overflow,
                has_overflow(lhs_value.alias.u, rhs_value.alias.u, result.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::neg:{
			operand_value_t value;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}
            operand_value_t result;
            if (value.type == register_type_t::floating_point) {
                result.alias.d = value.alias.d * -1.0;
            } else {
                int64_t negated_result = -static_cast<int64_t>(value.alias.u);
                result.alias.u = static_cast<uint64_t>(negated_result);
            }
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::shl:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = lhs_value.alias.u << rhs_value.alias.u;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::shr: {
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t result;
			result.alias.u = lhs_value.alias.u >> rhs_value.alias.u;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::rol:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t left_rotated_value;
			left_rotated_value.alias.u = rotl(lhs_value.alias.u, static_cast<uint8_t>(rhs_value.alias.u));
			if (!set_target_operand_value(r, inst, 0, left_rotated_value)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, left_rotated_value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(left_rotated_value, inst.size));
		}break;
		case op_codes::ror:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t right_rotated_value;
			right_rotated_value.alias.u = rotr(lhs_value.alias.u, static_cast<uint8_t>(rhs_value.alias.u));
			if (!set_target_operand_value(r, inst, 0, right_rotated_value)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, right_rotated_value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(right_rotated_value, inst.size));
		}break;
		case op_codes::and_op:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = lhs_value.alias.u & rhs_value.alias.u;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::or_op:{
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = lhs_value.alias.u | rhs_value.alias.u;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::xor_op: {
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 1, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, rhs_value)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = lhs_value.alias.u ^ rhs_value.alias.u;
			if (!set_target_operand_value(r, inst, 0, result)) {
				return false;
			}
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::not_op:{
			operand_value_t value;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}
            operand_value_t not_res;
			not_res.alias.u = ~value.alias.u;
			if (!set_target_operand_value(r, inst, 0, not_res)) {
				return false;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, not_res.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(not_res, inst.size));
		}break;
		case op_codes::bis:{
			operand_value_t value, bit_number;
			if (!get_operand_value(r, inst, 1, value))  {
				return false;
			}

			if (!get_operand_value(r, inst, 2, bit_number)) {
				return false;
			}
            operand_value_t result;
			uint64_t masked_value =static_cast<uint64_t > (1 << bit_number.alias.u);
            result.alias.u = value.alias.u | masked_value;
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
			operand_value_t value, bit_number;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, bit_number)) {
				return false;
			}
            operand_value_t result;
			uint64_t masked_value = static_cast<uint64_t>(~(1 << bit_number.alias.u));
            result.alias.u =value.alias.u & masked_value;
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
			operand_value_t value, mask;
			if (!get_operand_value(r, inst, 1, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, mask)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = value.alias.u & mask.alias.u;
			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::cmp: {
			operand_value_t lhs_value, rhs_value;
			if (!get_operand_value(r, inst, 0, lhs_value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, rhs_value)) {
				return false;
			}
            operand_value_t result;
			result.alias.u = lhs_value.alias.u - rhs_value.alias.u;
			registers_.flags(register_file_t::flags_t::overflow,
                             has_overflow(lhs_value.alias.u, rhs_value.alias.u, result.alias.u, inst.size));
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::subtract, true);
			registers_.flags(register_file_t::flags_t::carry, has_carry(result, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		}break;
		case op_codes::bz: {
			operand_value_t value, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (value.alias.u == 0) {
				registers_.r[register_pc].qw = address.alias.u;
			}
			registers_.flags(register_file_t::flags_t::zero, value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::carry, has_carry(value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::bnz: {
			operand_value_t value, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, address)) {
				return false;
			}

			if (value.alias.u != 0) {
				registers_.r[register_pc].qw = address.alias.u;
			}
			registers_.flags(register_file_t::flags_t::zero, value.alias.u == 0);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::carry, has_carry(value, inst.size));
			registers_.flags(register_file_t::flags_t::negative, is_negative(value, inst.size));
		}break;
		case op_codes::tbz: {
			operand_value_t value, mask, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, mask)) {
				return false;
			}
			if (!get_operand_value(r, inst, 2, address)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = value.alias.u & mask.alias.u;
			if (result.alias.u == 0) {
				registers_.r[register_pc].qw = address.alias.u;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));

		} break;
		case op_codes::tbnz: {
			operand_value_t value, mask, address;
			if (!get_operand_value(r, inst, 0, value)) {
				return false;
			}

			if (!get_operand_value(r, inst, 1, mask)) {
				return false;
			}

			if (!get_operand_value(r, inst, 2, address)) {
				return false;
			}
            operand_value_t result;
            result.alias.u = value.alias.u & mask.alias.u;
			if (result.alias.u != 0) {
				registers_.r[register_pc].qw = address.alias.u;
			}

			registers_.flags(register_file_t::flags_t::carry, false);
			registers_.flags(register_file_t::flags_t::subtract, false);
			registers_.flags(register_file_t::flags_t::overflow, false);
			registers_.flags(register_file_t::flags_t::zero, result.alias.u == 0);
			registers_.flags(register_file_t::flags_t::negative, is_negative(result, inst.size));
		}break;
		case op_codes::bne: {
			operand_value_t address;
			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::zero) == 0) {
				registers_.r[register_pc].qw = address.alias.u;
			}
		}break;
		case op_codes::beq: {
			operand_value_t address;
			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::zero)) {
				registers_.r[register_pc].qw = address.alias.u;
			}
		} break;
		case op_codes::bg: {
			operand_value_t address;
			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (!registers_.flags(register_file_t::flags_t::carry)
				&& !registers_.flags(register_file_t::flags_t::zero)) {
				registers_.r[register_pc].qw = address.alias.u;
			}
		}break;
		case op_codes::bge: {
			operand_value_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (!registers_.flags(register_file_t::flags_t::carry)) {
				registers_.r[register_pc].qw = address.alias.u;
			}
		}break;
		case op_codes::bl: {
			operand_value_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::carry)
				|| registers_.flags(register_file_t::flags_t::zero)) {
				registers_.r[register_pc].qw = address.alias.u;
			}
		}break;
		case op_codes::ble:{
			operand_value_t address;

			auto result = get_constant_address_or_pc_with_offset(r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			if (registers_.flags(register_file_t::flags_t::carry)) {
				registers_.r[register_pc].qw = address.alias.u;
			}
		}break;
		case op_codes::jsr: {
			push(registers_.r[register_pc].qw);
			operand_value_t address;
			auto result = get_constant_address_or_pc_with_offset( r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}

			registers_.r[register_pc].qw  = address.alias.u;
		}break;
		case op_codes::rts: {
            operand_value_t address;
			address.alias.u = pop();
            registers_.r[register_pc].qw  = address.alias.u;
		}break;
		case op_codes::jmp: {
			operand_value_t address;

			auto result = get_constant_address_or_pc_with_offset( r, inst, 0, inst_size, address);
			if (!result) {
				return false;
			}
            registers_.r[register_pc].qw  = address.alias.u;
		}break;
		case op_codes::swi:{
			operand_value_t index;
			if (!get_operand_value(r, inst, 0, index)) {
				return false;
			}
            operand_value_t swi_address;
			size_t swi_offset = sizeof(uint64_t) * index.alias.u;
			swi_address.alias.u = read(inst.size, swi_offset);
			if (swi_address.alias.u != 0) {
				/// TODO 恢复现场
				push(registers_.r[register_pc].qw);
                registers_.r[register_pc].qw = swi_address.alias.u;
			}
		}break;
		case op_codes::trap:{
			operand_value_t index;
			if (!get_operand_value(r, inst, 0, index)) {
				return false;
			}

			execute_trap(static_cast<uint8_t>(index.alias.u));

		}break;
		case op_codes::ffi:{
			operand_value_t address;
			if (!get_operand_value(r, inst, 0, address)) {
				return false;
			}

			auto it = foreign_functions_.find(reinterpret_cast<void*>(address.alias.u));
			if (it == foreign_functions_.end()) {
				execute_trap(trap_invalid_ffi_call);
				break;
			}
			auto func_signature = &it->second;
			func_signature->apply_calling_convention(call_vm_);

			dcReset(call_vm_);

            auto param_index = 0;
            int arg_count = pop();
            while (arg_count > 0) {
                auto &argument = func_signature->arguments[param_index];
                auto value = pop();
                if (argument.type == ffi_types_t::pointer_type) {
                    value += reinterpret_cast<uint64_t>(heap_);
                }

                argument.push(call_vm_, value);
                --arg_count;

                if (param_index < (int32_t)func_signature->arguments.size()) {
                    ++param_index;
                }
            }
            uint64_t result_value = func_signature->call(call_vm_, address.alias.u);
            if (func_signature->return_value.type != ffi_types_t::void_type) {
                push(result_value);
            }
		}break;
		case op_codes::meta: {
			operand_value_t meta_data_size;

			if (!get_operand_value(r, inst, 0, meta_data_size)) {
				return false;
			}

		}break;
		case op_codes::exit: {
			exited_ = true;
		}break;
        case op_codes::setz: {
            operand_value_t result;
            result.alias.u = registers_.flags(register_file_t::flags_t::zero) ? 1 : 0;
            if (!set_target_operand_value(r, inst, 0, result)) {
                return false;
            }

            break;
        }
        case op_codes::setnz: {
            operand_value_t result;
            result.alias.u = !registers_.flags(register_file_t::flags_t::zero) ? 1 : 0;
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
	uint64_t value = read(op_sizes::qword, registers_.r[register_sp].qw);
	registers_.r[register_sp].qw += sizeof(uint64_t);
	return value;
}

void terp::push(uint64_t value)
{
	registers_.r[register_sp].qw -= sizeof(uint64_t);
    write(op_sizes::qword, registers_.r[register_sp].qw, value);
}

void terp::dump_heap(uint64_t offset, size_t size)
{
	auto pMemory = formatter::Hex((void*)(heap_ + offset), size);
	fmt::print("{}\n",pMemory);
}

bool terp::get_operand_value(result& r, const instruction_t& instruction, uint8_t operand_index, operand_value_t& value) const
{
	auto& operand = instruction.operands[operand_index];
    value.type = operand.is_integer() ? register_type_t::integer : register_type_t::floating_point;
    if (operand.is_reg()) {
        auto reg_index = register_index(static_cast<registers_t>(operand.value.r),
            operand.is_integer() ? register_type_t::integer : register_type_t::floating_point);
        value.alias.u = registers_.r[reg_index].qw;
    } else {
        value.alias.u = operand.value.u;
    }
	return true;
}

bool terp::set_target_operand_value(result &r, const instruction_t &inst, uint8_t operand_index,
                                    const operand_value_t& value)
{
	auto& operand = inst.operands[operand_index];
    auto type = operand.is_integer() ? register_type_t::integer : register_type_t::floating_point;
    if (operand.is_reg()) {
        auto reg_index = register_index(static_cast<registers_t>(operand.value.r), type);
        set_zoned_value(registers_.r[reg_index], value.alias.u, inst.size);
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
	uint64_t value = read(op_sizes::qword, registers_.r[register_sp].qw);
	return value;
}

void terp::swi(uint8_t index, uint64_t address)
{
	size_t swi_address = interrupt_vector_table_start + (sizeof(uint64_t) * index);
    write(op_sizes::qword, swi_address, address);
}

std::vector<uint64_t> terp::jump_to_subroutine(result &r, uint64_t address)
{
	std::vector<uint64_t> return_values;

	auto return_address = registers_.r[register_pc].qw;
	push(return_address);
	registers_.r[register_pc].qw = address;

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
	uint64_t heap_vector_address = heap_vector_table_start
		+ (sizeof(uint64_t) * static_cast<uint8_t>(vector));
	return read(op_sizes::qword, heap_vector_address);
}

void terp::heap_vector(heap_vectors_t vector, uint64_t address)
{
	uint64_t heap_vector_address = heap_vector_table_start
		+ (sizeof(uint64_t) * static_cast<uint8_t>(vector));
    write(op_sizes::qword, heap_vector_address, address);
}

void terp::set_zoned_value(register_value_alias_t& reg, uint64_t value, op_sizes size)
{
	switch (size) {
		case op_sizes::byte: {
            reg.b = static_cast<uint8_t>(value);
			break;
		}
		case op_sizes::word: {
            reg.w = static_cast<uint16_t>(value);
			break;
		}
		case op_sizes::dword: {
            reg.dw = static_cast<uint32_t>(value);
			break;
		}
		default:
        case op_sizes::qword: {
            reg.qw = value;
            break;
        }
	}
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

bool terp::is_negative(operand_value_t value, op_sizes size)
{
	switch (size) {
		case op_sizes::byte: {
			return (value.alias.u & mask_byte_negative) != 0;
		}
		case op_sizes::word: {
			return (value.alias.u & mask_word_negative) != 0;
		}
		case op_sizes::dword: {
			return (value.alias.u & mask_dword_negative) != 0;
		}
		case op_sizes::qword:
		default:
			return (value.alias.u & mask_qword_negative) != 0;
	}
	return false;
}

bool terp::has_carry(operand_value_t value, op_sizes size)
{
	switch (size) {
		case op_sizes::byte:
			return value.alias.u > UINT8_MAX;
		case op_sizes::word:
			return value.alias.u > UINT16_MAX;
		case op_sizes::dword:
			return value.alias.u > UINT32_MAX;
		case op_sizes::qword:
		default:
			return value.alias.u > UINT64_MAX;
	}
}

bool terp::get_constant_address_or_pc_with_offset(result &r, const instruction_t &inst, uint8_t operand_index,
	uint64_t inst_size, operand_value_t &address)
{
	if (!get_operand_value(r, inst, operand_index, address)) {
		return false;
	}

	if (inst.operands_count >= 2) {
		operand_value_t offset;

		uint8_t offset_index = static_cast<uint8_t>(operand_index + 1);
		if (!get_operand_value(r, inst, offset_index, offset)) {
			return false;
		}

		if (inst.operands[offset_index].is_negative()) {
			address.alias.u -= offset.alias.u + inst_size;
		} else {
			address.alias.u += offset.alias.u - inst_size;
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

	auto func_ptr = signature.library->symbol_address(signature.symbol);
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

uint64_t terp::read(op_sizes size, uint64_t address) const
{
    uint8_t* relative_heap_ptr = heap_ + address;
    uint64_t result = 0;
    auto result_ptr = reinterpret_cast<uint8_t*>(&result);
    switch (size) {
        case op_sizes::none: {
            break;
        }
        case op_sizes::byte: {
            result = *relative_heap_ptr;
            break;
        }
        case op_sizes::word: {
            *(result_ptr + 0) = relative_heap_ptr[0];
            *(result_ptr + 1) = relative_heap_ptr[1];
            break;
        }
        case op_sizes::dword: {
            *(result_ptr + 0) = relative_heap_ptr[0];
            *(result_ptr + 1) = relative_heap_ptr[1];
            *(result_ptr + 2) = relative_heap_ptr[2];
            *(result_ptr + 3) = relative_heap_ptr[3];
            break;
        }
        case op_sizes::qword: {
            *(result_ptr + 0) = relative_heap_ptr[0];
            *(result_ptr + 1) = relative_heap_ptr[1];
            *(result_ptr + 2) = relative_heap_ptr[2];
            *(result_ptr + 3) = relative_heap_ptr[3];
            *(result_ptr + 4) = relative_heap_ptr[4];
            *(result_ptr + 5) = relative_heap_ptr[5];
            *(result_ptr + 6) = relative_heap_ptr[6];
            *(result_ptr + 7) = relative_heap_ptr[7];
            break;
        }
    }
    return result;
}

void terp::write(op_sizes size, uint64_t address, uint64_t value)
{
    uint8_t* relative_heap_ptr = heap_ + address;
    auto value_ptr = reinterpret_cast<uint8_t*>(&value);
    switch (size) {
        case op_sizes::none: {
            break;
        }
        case op_sizes::byte: {
            *relative_heap_ptr = static_cast<uint8_t>(value);
            break;
        }
        case op_sizes::word: {
            *(relative_heap_ptr + 0) = value_ptr[0];
            *(relative_heap_ptr + 1) = value_ptr[1];
            break;
        }
        case op_sizes::dword: {
            *(relative_heap_ptr + 0) = value_ptr[0];
            *(relative_heap_ptr + 1) = value_ptr[1];
            *(relative_heap_ptr + 2) = value_ptr[2];
            *(relative_heap_ptr + 3) = value_ptr[3];
            break;
        }
        case op_sizes::qword: {
            *(relative_heap_ptr + 0) = value_ptr[0];
            *(relative_heap_ptr + 1) = value_ptr[1];
            *(relative_heap_ptr + 2) = value_ptr[2];
            *(relative_heap_ptr + 3) = value_ptr[3];
            *(relative_heap_ptr + 4) = value_ptr[4];
            *(relative_heap_ptr + 5) = value_ptr[5];
            *(relative_heap_ptr + 6) = value_ptr[6];
            *(relative_heap_ptr + 7) = value_ptr[7];
            break;
        }
    }
}

void terp::set_pc_address(uint64_t address)
{
    registers_.r[register_pc].qw = address;
}

}