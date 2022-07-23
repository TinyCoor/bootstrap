//
// Created by 12132 on 2022/7/17.
//

#ifndef VM_INSTRUCTION_BLOCK_H_
#define VM_INSTRUCTION_BLOCK_H_
#include "instruction.h"
#include "instruction_builder.h"
#include "common/id_pool.h"
#include "label.h"
#include <set>
#include <vector>
#include <string>
namespace gfx {
enum class instruction_block_type_t {
    implicit,
    procedure
};

class instruction_block {
public:
    explicit instruction_block(instruction_block* parent, instruction_block_type_t type);

    virtual ~instruction_block();

    void rts();

    void dup();

    void nop();

    void exit();

    void swi(uint8_t index);

    void trap(uint8_t index);

    void disassemble();

    void clear_labels();

    void clear_blocks();

    void clear_instructions();

    /// load variations
    void load_to_ireg_u8(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset = 0);

    void load_to_ireg_u16(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset = 0);

    void load_to_ireg_u32(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset = 0);

    void load_to_ireg_u64(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset = 0);

    /// store
    void store_from_ireg_u8(i_registers_t src_reg, i_registers_t address_reg, int64_t offset = 0);

    void store_from_ireg_u16( i_registers_t src_reg, i_registers_t address_reg, int64_t offset = 0);

    void store_from_ireg_u32(i_registers_t src_reg, i_registers_t address_reg, int64_t offset = 0);

    void store_from_ireg_u64(i_registers_t src_reg, i_registers_t address_reg, int64_t offset = 0);
    // neg variations
    template<typename T>
    void neg(i_registers_t dest_reg, i_registers_t src_reg)
    {
        make_neg_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, src_reg);
    }

    void neg_u8(i_registers_t dest_reg, i_registers_t src_reg);

    void neg_u16(i_registers_t dest_reg, i_registers_t src_reg);

    void neg_u32(i_registers_t dest_reg, i_registers_t src_reg);

    void neg_u64(i_registers_t dest_reg, i_registers_t src_reg);
    /// move
    void move_f32_to_freg(f_registers_t dest_reg, float immediate);

    void move_f64_to_freg(f_registers_t dest_reg, double immediate);

    void move_u8_to_ireg(i_registers_t dest_reg, uint8_t immediate);

    void move_u16_to_ireg(i_registers_t dest_reg, uint16_t immediate);

    void move_u32_to_ireg(i_registers_t dest_reg, uint32_t immediate);

    void move_u64_to_ireg(i_registers_t dest_reg, uint64_t immediate);

    void move_ireg_to_ireg(i_registers_t dest_reg, i_registers_t src_reg);
    // inc variations
    void inc_u8(i_registers_t reg);

    void inc_u16(i_registers_t reg);

    void inc_u32(i_registers_t reg);

    void inc_u64(i_registers_t reg);

    // dec variations
    void dec_u8(i_registers_t reg);

    void dec_u16(i_registers_t reg);

    void dec_u32(i_registers_t reg);

    void dec_u64(i_registers_t reg);

    // mul variations
    void mul_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t multiplicand_reg, i_registers_t multiplier_reg);

    void mul_ireg_by_ireg_u16(i_registers_t dest_reg, i_registers_t multiplicand_reg, i_registers_t multiplier_reg);

    void mul_ireg_by_ireg_u32(i_registers_t dest_reg, i_registers_t multiplicand_reg, i_registers_t multiplier_reg);

    void mul_ireg_by_ireg_u64(i_registers_t dest_reg,i_registers_t multiplicand_reg, i_registers_t multiplier_reg);

    // add variations
    template<typename T>
    void add_ireg_by_ireg(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg)
    {
        make_add_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, augend_reg, addened_reg);
    }
    void add_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    void add_ireg_by_ireg_u16(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    void add_ireg_by_ireg_u32(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    void add_ireg_by_ireg_u64( i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    // sub variations
    void sub_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    void sub_ireg_by_ireg_u16(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    void sub_ireg_by_ireg_u32(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    void sub_ireg_by_ireg_u64(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg);

    // div variations
    void div_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void div_ireg_by_ireg_u16(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void div_ireg_by_ireg_u32(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void div_ireg_by_ireg_u64(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    // mod variations
    void mod_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void mod_ireg_by_ireg_u16(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void mod_ireg_by_ireg_u32(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void mod_ireg_by_ireg_u64(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    // swap variations
    void swap_ireg_with_ireg_u8(i_registers_t dest_reg, i_registers_t src_reg);

    void swap_ireg_with_ireg_u16(i_registers_t dest_reg, i_registers_t src_reg);

    void swap_ireg_with_ireg_u32(i_registers_t dest_reg, i_registers_t src_reg);

    void swap_ireg_with_ireg_u64(i_registers_t dest_reg, i_registers_t src_reg);

    // test mask for zero and branch
    void test_mask_branch_if_zero_u8(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void test_mask_branch_if_zero_u16(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void test_mask_branch_if_zero_u32(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void test_mask_branch_if_zero_u64(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    // test mask for non-zero and branch
    void test_mask_branch_if_not_zero_u8(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void test_mask_branch_if_not_zero_u16(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void test_mask_branch_if_not_zero_u32(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void test_mask_branch_if_not_zero_u64(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg);

    void push_f32(float value);

    void push_f64(double value);

    void push_u8(uint8_t value);

    void push_u16(uint16_t value);

    void push_u32(uint32_t value);

    void push_u64(uint64_t value);

    void push_u8(i_registers_t reg);

    void push_u16(i_registers_t reg);

    void push_f32(f_registers_t reg);

    void push_u32(i_registers_t reg);

    void push_f64(f_registers_t reg);

    void push_u64(i_registers_t reg);

    // register allocators
    void free_ireg(i_registers_t reg);

    void free_freg(f_registers_t reg);

    void pop_u8(i_registers_t reg);

    void pop_f32(f_registers_t reg);

    void pop_f64(f_registers_t reg);

    void pop_u16(i_registers_t reg);

    void pop_u32(i_registers_t reg);

    void pop_u64(i_registers_t reg);

    i_registers_t allocate_ireg();

    f_registers_t allocate_freg();

    bool reserve_ireg(i_registers_t reg);

    bool reserve_freg(f_registers_t reg);

    void jump_indirect(i_registers_t reg);

    [[nodiscard]] instruction_block_type_t type() const;

    void add_block(instruction_block* block);

    void remove_block(instruction_block* block);

    void call(const std::string& proc_name);

    void call_foreign(const std::string& proc_name);

    label* make_label(const std::string& name);

    void jump_direct(const std::string& label_name);

    void move_label_to_ireg(i_registers_t dest_reg, const std::string& label_name);

    instruction_block* parent();
private:
    void make_neg_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void make_inc_instruction(op_sizes size, i_registers_t reg);

    void make_dec_instruction(op_sizes size, i_registers_t reg);

    void make_load_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t address_reg, int64_t offset);

    void make_store_instruction(op_sizes size, i_registers_t src_reg, i_registers_t address_reg, int64_t offset);

    void make_swap_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void make_pop_instruction(op_sizes size, i_registers_t dest_reg);

    void make_pop_instruction(op_sizes size, f_registers_t dest_reg);

    void make_move_instruction(op_sizes size, i_registers_t dest_reg, uint64_t value);

    void make_move_instruction(op_sizes size, f_registers_t dest_reg, double value);

    void make_move_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void disassemble(instruction_block* block);

    void make_float_constant_push_instruction(op_sizes size, double value);

    void make_integer_constant_push_instruction(op_sizes size, uint64_t value);

    void make_add_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t augend_reg,
        i_registers_t addend_reg);

    void make_sub_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t minuend_reg,
        i_registers_t subtrahend_reg);

    label_ref_t* find_unresolved_label_up(id_t id);

    label* find_label_up(const std::string& label_name);

    void make_push_instruction(op_sizes size, i_registers_t reg);

    void make_push_instruction(op_sizes size, f_registers_t reg);

    label_ref_t* make_unresolved_label_ref(const std::string& label_name);

private:
    instruction_block* parent_ = nullptr;
    instruction_block_type_t type_;
    std::vector<instruction_block*> blocks_ {};
    std::vector<instruction_t> instructions_ {};
    std::map<std::string, label*> labels_ {};
    std::set<f_registers_t> used_float_registers_ {};
    std::set<i_registers_t> used_integer_registers_ {};
    std::map<std::string, size_t> label_to_instruction_map_{};
    std::unordered_map<id_t, label_ref_t> unresolved_labels_ {};
    std::unordered_map<std::string, id_t> label_to_unresolved_ids_ {};
};
}

#endif // VM_INSTRUCTION_BLOCK_H_
