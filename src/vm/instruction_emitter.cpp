//
// Created by 12132 on 2022/3/28.
//

#include "instruction_emitter.h"
namespace gfx {

size_t instruction_emitter::size() const
{
	size_t size = 0;
	for (const auto& inst : inst_) {
		size += inst.encoding_size();
	}
	return size;
}

void instruction_emitter::nop()
{
	instruction_t no_op;
	no_op.op = op_codes::nop;
	inst_.emplace_back(no_op);
}

void instruction_emitter::dup()
{
	instruction_t dup_op;
	dup_op.op = op_codes::dup;
	inst_.emplace_back(dup_op);
}

void instruction_emitter::rts()
{
	instruction_t rts_op;
	rts_op.op = op_codes::rts;
	inst_.emplace_back(rts_op);
}

void instruction_emitter::swi(uint8_t index) {
	instruction_t swi_op;
	swi_op.op = op_codes::swi;
	swi_op.size = op_sizes::byte;
	swi_op.operands_count =1;
	swi_op.operands[0].type = operand_encoding_t::flags::integer;
	swi_op.operands[0].value.u64 = index;
	inst_.emplace_back(swi_op);
}

void instruction_emitter::trap(uint8_t index)
{
	instruction_t trap_op;
	trap_op.op = op_codes::trap;
	trap_op.size = op_sizes::byte;
	trap_op.operands_count = 1;
	trap_op.operands[0].type = operand_encoding_t::flags::integer;
	trap_op.operands[0].value.u64 = index;
	inst_.push_back(trap_op);
}

void instruction_emitter::exit()
{
	instruction_t exit_op;
	exit_op.op = op_codes::exit;
	inst_.emplace_back(exit_op);
}

void instruction_emitter::load_with_offset_to_register(op_sizes size, i_registers_t source_index,
													   i_registers_t target_index, uint64_t offset)
{
	instruction_t load_op;
	load_op.op = op_codes::load;
	load_op.size = size;
	load_op.operands_count = 3;
	load_op.operands[0].type = operand_encoding_t::flags::integer |operand_encoding_t::flags::reg ;
	load_op.operands[0].value.r8 = target_index;
	load_op.operands[1].type = operand_encoding_t::flags::integer |operand_encoding_t::flags::reg;
	load_op.operands[1].value.r8 = source_index;
	load_op.operands[2].type = operand_encoding_t::flags::integer |operand_encoding_t::flags::constant;
	load_op.operands[2].value.u64 = offset;
	inst_.push_back(load_op);
}


void instruction_emitter::add_int_register_to_register(op_sizes size, i_registers_t target_index,
													   i_registers_t lhs_index, i_registers_t rhs_index)
{
	instruction_t add_op;
	add_op.op = op_codes::add;
	add_op.size= size;
	add_op.operands_count = 3;
	add_op.operands[0].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::reg;
	add_op.operands[0].value.r8 = target_index;
	add_op.operands[1].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::reg;
	add_op.operands[1].value.r8 = lhs_index;
	add_op.operands[2].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::reg;
	add_op.operands[2].value.r8 = rhs_index;
	inst_.emplace_back(add_op);
}

void instruction_emitter::load_stack_offset_to_register(op_sizes size ,i_registers_t target_index, uint64_t offset)
{
	instruction_t load_op;
	load_op.op = op_codes::load;
	load_op.size = size;
	load_op.operands_count = 3;
	load_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	load_op.operands[0].value.r8 = target_index;
	load_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	load_op.operands[1].value.r8 = i_registers_t::sp;
	load_op.operands[2].type =operand_encoding_t::integer | operand_encoding_t::constant;
	load_op.operands[2].value.u64 = offset;
	inst_.emplace_back(load_op);
}

void instruction_emitter::store_register_to_stack_offset(op_sizes size ,i_registers_t source_index, uint64_t offset)
{
	instruction_t store_op;
	store_op.op = op_codes::store;
	store_op.size = size;
	store_op.operands_count = 3;
	store_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	store_op.operands[0].value.r8 = source_index;
	store_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	store_op.operands[1].value.r8 = i_registers_t::sp;
	store_op.operands[2].type = operand_encoding_t::integer | operand_encoding_t::constant;
	store_op.operands[2].value.u64 = offset;
	inst_.emplace_back(store_op);
}

void instruction_emitter::divide_int_register_to_register(op_sizes size, i_registers_t target_index,
														  i_registers_t lhs_index, i_registers_t rhs_index)
{
	instruction_t div_op;
	div_op.op = op_codes::div;
	div_op.size = size;
	div_op.operands_count = 3;
	div_op.operands[0].value.r8 = target_index;
	div_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	div_op.operands[1].value.r8 = lhs_index;
	div_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	div_op.operands[2].value.r8 = rhs_index;
	div_op.operands[2].type = operand_encoding_t::integer | operand_encoding_t::reg;
	inst_.push_back(div_op);
}

void instruction_emitter::store_with_offset_from_register(op_sizes size ,i_registers_t source_index,
														  i_registers_t target_index, uint64_t offset)
{
	instruction_t store_op;
	store_op.op = op_codes::store;
	store_op.size = size;
	store_op.operands_count = 3;
	store_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	store_op.operands[0].value.r8 = source_index;
	store_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	store_op.operands[1].value.r8 = target_index;
	store_op.operands[2].type = operand_encoding_t::integer | operand_encoding_t::constant;
	store_op.operands[2].value.u64 = offset;
	inst_.push_back(store_op);
}

void instruction_emitter::subtract_int_register_to_register(op_sizes size, i_registers_t target_index,
															i_registers_t lhs_index, i_registers_t rhs_index)
{
	instruction_t sub_op;
	sub_op.op = op_codes::sub;
	sub_op.size = size;
	sub_op.operands_count = 3;
	sub_op.operands[0].value.r8 = target_index;
	sub_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	sub_op.operands[1].value.r8 = lhs_index;
	sub_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	sub_op.operands[2].value.r8 = rhs_index;
	sub_op.operands[2].type = operand_encoding_t::integer | operand_encoding_t::reg;
	inst_.push_back(sub_op);
}

void instruction_emitter::subtract_int_constant_from_register(op_sizes size, i_registers_t target_index,
															  i_registers_t lhs_index, uint64_t rhs_value)
{
	instruction_t sub_op;
	sub_op.op = op_codes::sub;
	sub_op.operands_count = 1;
	sub_op.size = size;
	sub_op.operands_count = 3;
	sub_op.operands[0].value.r8 = target_index;
	sub_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	sub_op.operands[1].value.r8 = lhs_index;
	sub_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	sub_op.operands[2].type = operand_encoding_t::integer | operand_encoding_t::constant;
	sub_op.operands[2].value.u64 = rhs_value;
	inst_.push_back(sub_op);
}

void instruction_emitter::multiply_int_register_to_register(op_sizes size, i_registers_t target_index,
	i_registers_t lhs_index, i_registers_t rhs_index)
{
	instruction_t mul_op;
	mul_op.op = op_codes::mul;
	mul_op.size = size;
	mul_op.operands_count = 3;
	mul_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	mul_op.operands[0].value.r8 = target_index;
	mul_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	mul_op.operands[1].value.r8 = lhs_index;
	mul_op.operands[2].type = operand_encoding_t::integer | operand_encoding_t::reg;
	mul_op.operands[2].value.r8 = rhs_index;
	inst_.emplace_back(mul_op);
}

void instruction_emitter::move_int_constant_to_register(op_sizes size, uint64_t value, i_registers_t index)
{
	instruction_t move_op;
	move_op.op = op_codes::move;
	move_op.size = size;
	move_op.operands_count = 2;
	move_op.operands[0].value.u64 = value;
	move_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	move_op.operands[1].value.r8 = index;
	move_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::reg;
	inst_.emplace_back(move_op);
}
void instruction_emitter::jump_direct(uint64_t address)
{
	instruction_t jmp_op;
	jmp_op.op = op_codes::jmp;
	jmp_op.size = op_sizes::qword;
	jmp_op.operands_count = 1;
	jmp_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	jmp_op.operands[0].value.u64 = address;
	inst_.emplace_back(jmp_op);
}

void instruction_emitter::pop_float_register(i_registers_t index)
{
	instruction_t pop_op;
	pop_op.op = op_codes::pop;
	pop_op.operands_count = 1;
	pop_op.operands[0].type = operand_encoding_t::reg;
	pop_op.operands[0].value.r8 = index;
	inst_.emplace_back(pop_op);
}

void instruction_emitter::push_float_constant(double value)
{
	instruction_t push_op;
	push_op.op = op_codes::push;
	push_op.operands_count = 1;
	push_op.operands[0].type = operand_encoding_t::constant;
	push_op.operands[0].value.d64 = value;
	inst_.emplace_back(push_op);
}


void instruction_emitter::dec(op_sizes size, i_registers_t index)
{
	instruction_t dec_op;
	dec_op.op = op_codes::dec;
	dec_op.operands_count = 1;
	dec_op.size = size;
	dec_op.operands[0].value.r8 = index;
	dec_op.operands[0].type =operand_encoding_t::integer | operand_encoding_t::reg;
	inst_.emplace_back(dec_op);
}

void instruction_emitter::inc(op_sizes size, i_registers_t index)
{
	instruction_t inc_op;
	inc_op.op = op_codes::dec;
	inc_op.operands_count = 1;
	inc_op.size = size;
	inc_op.operands[0].value.r8 = index;
	inc_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	inst_.emplace_back(inc_op);
}


void instruction_emitter::jump_subroutine_indirect(i_registers_t index)
{
	instruction_t jsr_op;
	jsr_op.op = op_codes::jsr;
	jsr_op.size = op_sizes::qword;
	jsr_op.operands_count = 1;
	jsr_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	jsr_op.operands[0].value.r8 = index;
	inst_.emplace_back(jsr_op);
}

void instruction_emitter::jump_subroutine_direct(uint64_t address)
{
	instruction_t jsr_op;
	jsr_op.op = op_codes::jsr;
	jsr_op.size = op_sizes::qword;
	jsr_op.operands_count = 1;
	jsr_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	jsr_op.operands[0].value.u64 = address;
	inst_.emplace_back(jsr_op);
}

void instruction_emitter::pop_int_register(op_sizes size, i_registers_t index)
{
	instruction_t pop_op;
	pop_op.op = op_codes::pop;
	pop_op.size = size;
	pop_op.operands_count = 1;
	pop_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	pop_op.operands[0].value.r8 = index;
	inst_.emplace_back(pop_op);
}

void instruction_emitter::push_int_register(op_sizes size, i_registers_t index)
{
	instruction_t push_op;
	push_op.op = op_codes::push;
	push_op.size = size;
	push_op.operands_count = 1;
	push_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	push_op.operands[0].value.r8 = index;
	inst_.emplace_back(push_op);
}

void instruction_emitter::push_int_constant(op_sizes size, uint64_t value)
{
	instruction_t push;
	push.op = op_codes::push;
	push.size = size;
	push.operands_count = 1;
	push.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	push.operands[0].value.u64 = value;
	inst_.emplace_back(push);
}


bool instruction_emitter::encode(result &r, terp &terp)
{
	size_t offset = 0;
	for (auto& inst : inst_ ) {
		auto inst_size = inst.encode(r, terp.heap(), start_address_ + offset);
		if (inst_size == 0){
			return false;
		}
		offset += inst_size;
	}
	return true;
}

void instruction_emitter::compare_int_register_to_register(op_sizes size, i_registers_t lhs_index,
														   i_registers_t rhs_index)
{
	instruction_t compare_op;
	compare_op.op = op_codes::cmp;
	compare_op.size = size;
	compare_op.operands_count = 2;
	compare_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	compare_op.operands[0].value.r8 = lhs_index;
	compare_op.operands[1].type =operand_encoding_t::integer | operand_encoding_t::reg;
	compare_op.operands[1].value.r8= rhs_index;
	inst_.push_back(compare_op);
}


void instruction_emitter::compare_int_register_to_constant(op_sizes sizes, i_registers_t index, uint64_t value)
{
	instruction_t compare_op;
	compare_op.op = op_codes::cmp;
	compare_op.size = sizes;
	compare_op.operands_count = 2;
	compare_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	compare_op.operands[0].value.r8 = index;
	compare_op.operands[1].type = operand_encoding_t::integer | operand_encoding_t::constant;
	compare_op.operands[1].value.u64 = value;
	inst_.push_back(compare_op);
}


void instruction_emitter::branch_if_equal(uint64_t address)
{
	instruction_t branch_op;
	branch_op.op = op_codes::beq;
	branch_op.size = op_sizes::qword;
	branch_op.operands_count = 1;
	branch_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	branch_op.operands[0].value.u64 = address;
	inst_.push_back(branch_op);
}

void instruction_emitter::branch_if_not_equal(uint64_t address)
{
	instruction_t branch_op;
	branch_op.op = op_codes::bne;
	branch_op.size = op_sizes::qword;
	branch_op.operands_count = 1;
	branch_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	branch_op.operands[0].value.u64 = address;
	inst_.push_back(branch_op);
}

void instruction_emitter::jump_subroutine_pc_relative(op_sizes size, operand_encoding_t::flags offset_type, uint64_t offset) {
	instruction_t jsr_op;
	jsr_op.op = op_codes::jsr;
	jsr_op.size = size;
	jsr_op.operands_count = 2;
	jsr_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::reg;
	jsr_op.operands[0].value.r8 = i_registers_t::pc;
	jsr_op.operands[1].type = offset_type | operand_encoding_t::flags::integer;
	jsr_op.operands[1].value.u64 = offset;
	inst_.push_back(jsr_op);
}
void instruction_emitter::branch_if_greater(uint64_t address) {
	instruction_t branch_op;
	branch_op.op = op_codes::bg;
	branch_op.size = op_sizes::qword;
	branch_op.operands_count = 1;
	branch_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	branch_op.operands[0].value.u64 = address;
	inst_.push_back(branch_op);
}

void instruction_emitter::branch_if_lesser(uint64_t address) {
	instruction_t branch_op;
	branch_op.op = op_codes::bl;
	branch_op.size = op_sizes::qword;
	branch_op.operands_count = 1;
	branch_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	branch_op.operands[0].value.u64 = address;
	inst_.push_back(branch_op);
}

void instruction_emitter::branch_if_lesser_or_equal(uint64_t address) {
	instruction_t branch_op;
	branch_op.op = op_codes::ble;
	branch_op.size = op_sizes::qword;
	branch_op.operands_count = 1;
	branch_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	branch_op.operands[0].value.u64 = address;
	inst_.push_back(branch_op);
}
void instruction_emitter::branch_if_greater_or_equal(uint64_t address) {
	instruction_t branch_op;
	branch_op.op = op_codes::bge;
	branch_op.size = op_sizes::qword;
	branch_op.operands_count = 1;
	branch_op.operands[0].type = operand_encoding_t::integer | operand_encoding_t::constant;
	branch_op.operands[0].value.u64 = address;
	inst_.push_back(branch_op);
}

}