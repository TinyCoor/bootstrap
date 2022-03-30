//
// Created by 12132 on 2022/3/28.
//

#ifndef BOOTSTRAP__INSTRUCTION_EMITTER_H_
#define BOOTSTRAP__INSTRUCTION_EMITTER_H_
#include "instruction.h"
#include "terp.h"
#include <vector>
namespace gfx {
class instruction_emitter {
public:
	explicit instruction_emitter(uint64_t address): start_address_(address) {}
	size_t size() const;
	uint64_t end_address() const { return start_address_ + size();};
	uint64_t start_address() const { return start_address_; }
	inline instruction_t& operator[](size_t index) { return inst_[index];};
	bool encode(result& r, terp& terp);

	void rts();

	void nop();

	void exit();

	/// dec r0
	void dec(op_sizes size, uint8_t index);

	/// inc r0
	void inc(op_sizes size, uint8_t index);

	void compare_int_register_to_constant(op_sizes sizes, uint8_t index, uint64_t value);

	void compare_int_register_to_register(op_sizes size, uint8_t lhs_index, uint8_t rhs_index);

	void branch_if_equal(uint64_t address);

	void branch_if_not_equal(uint64_t address);

	///  add target_register src1_register src2_register
	///
	void add_int_register_to_register(op_sizes size, uint8_t target_index, uint8_t lhs_index, uint8_t rhs_index);

	///  div target_register src1_register src2_register
	///
	void divide_int_register_to_register(op_sizes size, uint8_t target_index, uint8_t lhs_index, uint8_t rhs_index);

	///  mul target_register src1_register src2_register
	///
	void multiply_int_register_to_register(op_sizes size, uint8_t target_index, uint8_t lhs_index, uint8_t rhs_index);

	///  sub target_register src1_register src2_register
	///
	void subtract_int_register_to_register(op_sizes size, uint8_t target_index, uint8_t lhs_index, uint8_t rhs_index);

	void subtract_int_constant_from_register(op_sizes size, uint8_t target_index, uint8_t lhs_index, uint64_t rhs_value);

	/// load target_register  source_register offset
	/// load r0 sp #$8
	void load_with_offset_to_register(uint8_t source_index, uint8_t target_index, uint64_t offset);

	void move_int_constant_to_register(op_sizes size, uint64_t value, uint8_t index);

	void load_stack_offset_to_register(uint8_t target_index, uint64_t offset);

	void store_register_to_stack_offset(uint8_t source_index, uint64_t offset);

	/// store
	void store_with_offset_from_register(uint8_t source_index, uint8_t target_index, uint64_t offset);

	void jump_direct(uint64_t address);

	void push_float_constant( double value);

	void pop_float_register(uint8_t index);

	void jump_subroutine_indirect(uint8_t index);

	void jump_subroutine_direct(uint64_t address);

	void pop_int_register(op_sizes size, uint8_t index);

	/// push
	void push_int_register(op_sizes size, uint8_t index);

	/// push
	void push_int_constant( op_sizes size, uint64_t value);
private:
	uint64_t start_address_ = 0;
	std::vector<instruction_t> inst_{};
};
}
#endif //BOOTSTRAP__INSTRUCTION_EMITTER_H_
