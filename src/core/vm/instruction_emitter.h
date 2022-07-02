//
// Created by 12132 on 2022/3/28.
//

#ifndef INSTRUCTION_EMITTER_H_
#define INSTRUCTION_EMITTER_H_
#include "instruction.h"
#include "terp.h"
#include <vector>
namespace gfx {
class instruction_emitter {
public:
	instruction_emitter();

	~instruction_emitter();

	size_t size() const;

	void clear();

	size_t index() const;

	void rts();

	void dup();

	void exit();

	void nop();

	void trap(uint8_t index);

	void swi(uint8_t index);

	instruction_t& operator[](size_t index);

	bool encode(result& r, terp& terp, uint64_t address);

	/// dec r0
	void dec(op_sizes size, i_registers_t index);

	/// inc r0
	void inc(op_sizes size, i_registers_t index);

	void branch_if_equal(uint64_t address);

	void branch_if_greater(uint64_t address);

	void branch_if_lesser(uint64_t address);

	void branch_if_lesser_or_equal(uint64_t address);

	void branch_if_greater_or_equal(uint64_t address);

	void branch_if_not_equal(uint64_t address);

	void branch_pc_relative_if_equal(op_sizes size, operand_encoding_t::flags offset_type, uint64_t offset);

	void branch_pc_relative_if_not_equal(op_sizes size, operand_encoding_t::flags offset_type, uint64_t offset);

	void meta(uint32_t line, uint16_t column, const std::string& file_name, const std::string& symbol_name);

	void swap_int_register(op_sizes size, i_registers_t target_index, i_registers_t source_index);

	void compare_int_register_to_constant(op_sizes sizes, i_registers_t index, uint64_t value);

	void jump_direct(uint64_t address);

	void jump_pc_relative(op_sizes size, operand_encoding_t::flags offset_type, uint64_t offset);

	void jump_subroutine_pc_relative(op_sizes size, operand_encoding_t::flags offset_type, uint64_t offset);

	void jump_subroutine_direct(uint64_t address);

	void jump_subroutine_indirect(i_registers_t index);

	void push_float_constant(double value);

	void pop_float_register(i_registers_t index);

	void pop_int_register(op_sizes size, i_registers_t index);

	/// push
	void push_int_register(op_sizes size, i_registers_t index);

	/// push
	void push_int_constant(op_sizes size, uint64_t value);

	void move_int_constant_to_register(op_sizes size, uint64_t value, i_registers_t index);

	void compare_int_register_to_register(op_sizes size, i_registers_t lhs_index, i_registers_t rhs_index);

	void load_stack_offset_to_register(op_sizes size, i_registers_t target_index, uint64_t offset);

	void store_register_to_stack_offset(op_sizes size, i_registers_t source_index, uint64_t offset);

	///  add target_register src1_register src2_register
	void add_int_register_to_register(op_sizes size, i_registers_t target_index, i_registers_t lhs_index,
		i_registers_t rhs_index);

	///  div target_register src1_register src2_register
	void divide_int_register_to_register(op_sizes size, i_registers_t target_index, i_registers_t lhs_index,
		i_registers_t rhs_index);

	///  mul target_register src1_register src2_register
	void multiply_int_register_to_register(op_sizes size, i_registers_t target_index, i_registers_t lhs_index,
		i_registers_t rhs_index);

	///  sub target_register src1_register src2_register
	void subtract_int_register_to_register(op_sizes size, i_registers_t target_index, i_registers_t lhs_index,
		i_registers_t rhs_index);

	void subtract_int_constant_from_register(op_sizes size, i_registers_t target_index, i_registers_t lhs_index,
		uint64_t rhs_value);

	/// load target_register source_register offset
	void load_with_offset_to_register(op_sizes size, i_registers_t source_index, i_registers_t target_index,
		uint64_t offset);
	/// store
	void store_with_offset_from_register(op_sizes size, i_registers_t source_index, i_registers_t target_index,
		uint64_t offset);

private:
	std::vector<instruction_t> inst_{};
	std::vector<meta_information_t> meta_information_list_ {};
};
}
#endif // INSTRUCTION_EMITTER_H_
