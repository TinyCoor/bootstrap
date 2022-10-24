//
// Created by 12132 on 2022/7/17.
//

#ifndef VM_INSTRUCTION_BLOCK_H_
#define VM_INSTRUCTION_BLOCK_H_

#include "label.h"
#include "block_entry.h"
#include "instruction_builder.h"
#include "register_allocator.h"
#include "common/id_pool.h"
#include <set>
#include <any>
#include <vector>
#include "stack_frame.h"
#include "assembly_listing.h"
namespace gfx {
enum class instruction_block_type_t {
    basic,
    procedure
};
struct label_ref_t {
    id_t id;
    std::string name;
    label* resolved = nullptr;
};

class instruction_block {
public:
    using block_predicate_visitor_callable = std::function<bool (instruction_block*)>;

    explicit instruction_block(instruction_block* parent, instruction_block_type_t type);

    virtual ~instruction_block();

/// block support
public:
    void memo();

    void clear_blocks();

    void clear_entries();

    instruction_block* parent();

    stack_frame_t* stack_frame();

    block_entry_t* current_entry();

    instruction_block_type_t type() const;

    void add_block(instruction_block* block);

    void disassemble();

    void remove_block(instruction_block* block);

    label* make_label(const std::string& name);

    listing_source_file_t* source_file();

    void source_file(listing_source_file_t* value);

    std::vector<label_ref_t*> label_references();

    std::vector<block_entry_t>& entries();

    label* find_label(const std::string& name);

    const std::vector<instruction_block*>& blocks() const;

    bool walk_blocks(const block_predicate_visitor_callable& callable);

    label_ref_t* find_unresolved_label_up(id_t id);

    void make_block_entry(const align_t &data);

    void make_block_entry(const section_t &data);

    void make_block_entry(const instruction_t& inst);

    void make_block_entry(const data_definition_t& data);

    template <typename T>
    T* find_in_blocks(const std::function<T* (instruction_block*)>& callable)
    {
        std::stack<instruction_block*> block_stack {};
        block_stack.push(this);
        while (!block_stack.empty()) {
            auto block = block_stack.top();
            auto found = callable(block);
            if (found != nullptr)  {
                return found;
            }
            block_stack.pop();
            for (auto child_block : block->blocks()) {
                block_stack.push(child_block);
            }
        }
        return nullptr;
    }
// data definitions
public:
    void align(uint8_t size);

    void section(section_t type);

    void reserve_byte(size_t count);

    void reserve_word(size_t count);

    void reserve_dword(size_t count);

    void reserve_qword(size_t count);

    void string(const std::string& value);

    void bytes(const std::vector<uint8_t> &value);

    void words(const std::vector<uint16_t> &value);

    void dwords(const std::vector<uint32_t> &value);

    void qwords(const std::vector<uint64_t> &value);

/// instruction
public:
    void rts();

    void dup();

    void nop();

    void exit();

    void swi(uint8_t index);

    void trap(uint8_t index);

    // setxx
    void setz(const register_t& dest_reg);

    void setnz(const register_t& dest_reg);

    // branches
    void bne(const std::string& label_name);

    void beq(const std::string& label_name);

    void jump_indirect(const register_t& reg);

    // convert
    void convert(const register_t& dest_reg, const register_t& src_reg);

    // dec variations
    void dec(const register_t& reg);

    // inc variations
    void inc(const register_t& reg);

    void push(const register_t &reg);

    void pop(const register_t &reg);

    void call(const std::string& proc_name);

    void move_reg_to_reg(const register_t& dest_reg, const register_t& src_reg);

    void moves_reg_to_reg(const register_t& dest_reg, const register_t& src_reg);

    void movez_reg_to_reg(const register_t& dest_reg, const register_t& src_reg);

    // cmp variations
    void cmp(const register_t& lhs_reg, const register_t& rhs_reg);

    // neg variations
    void neg(const register_t& dest_reg, const register_t& src_reg);

    // not variations
    void not_op(const register_t& dest_reg, const register_t& src_reg);

    /// move
    void move_constant_to_reg(const register_t& dest_reg, uint64_t immediate);

    void move_constant_to_reg(const register_t& dest_reg, double immediate);

    /// load variations
    void load_to_reg(const register_t& dest_reg, const register_t& address_reg, int64_t offset = 0);

    /// store
    void store_from_reg(const register_t& address_reg, const register_t& src_reg, int64_t offset = 0);

    // or variations
    void or_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    // xor variations
    void xor_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    // and variations
    void and_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    // shl variations
    void shl_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    // shr variations
    void shr_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    // rol variations
    void rol_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    // ror variations
    void ror_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg);

    void mul_reg_by_reg(const register_t& dest_reg, const register_t& multiplicand_reg, const register_t& multiplier_reg);

    // add variations
    void add_reg_by_reg(const register_t& dest_reg, const register_t& augend_reg, const register_t& addened_reg);

    // sub variations
    void sub_reg_by_reg(const register_t& dest_reg, const register_t& augend_reg, const register_t& addened_reg);

    void sub_reg_by_immediate(const register_t& dest_reg, const register_t& minuend_reg, uint64_t subtrahend_immediate);
    // div variations
    void div_reg_by_reg(const register_t& dest_reg, const register_t& dividend_reg, const register_t& divisor_reg);

    // mod variations
    void mod_reg_by_reg(const register_t& dest_reg, const register_t& dividend_reg, const register_t& divisor_reg);

    // swap variations
    void swap_reg_with_reg(const register_t& dest_reg, const register_t& src_reg);

    // test mask for zero and branch
    void test_mask_branch_if_zero(const register_t& value_reg, const register_t& mask_reg, const register_t& address_reg);

    // test mask for non-zero and branch
    void test_mask_branch_if_not_zero(const register_t& value_reg, const register_t& mask_reg, const register_t& address_reg);

    ///
    template<class T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
    void push_constant(T value)
    {
        make_float_constant_push_instruction(TypeToOpSize::ToOpSize<T>(), value);
    }

    template<class T>
    void push_constant(T value)
    {
        make_integer_constant_push_instruction(TypeToOpSize::ToOpSize<T>(), value);
    }

    void call_foreign(uint64_t proc_address);

    void jump_direct(const std::string& label_name);

    void move_label_to_reg(const register_t& dest_reg, const std::string& label_name);

    void move_label_to_reg_with_offset(const register_t& dest_reg, const std::string& label_name,
        uint64_t offset);

private:
    void make_pop_instruction(const register_t& dest_reg);

    void make_push_instruction(const register_t& reg);

    void make_inc_instruction(op_sizes size, const register_t& reg);

    void make_dec_instruction(op_sizes size, const register_t& reg);

    void make_shl_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& amount_reg);

    void make_rol_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& amount_reg);

    void make_shr_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& amount_reg);

    void make_ror_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& amount_reg);

    void make_and_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& mask_reg);

    void make_xor_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& mask_reg);

    void make_or_instruction(op_sizes size, const register_t& dest_reg, const register_t& value_reg, const register_t& mask_reg);

    void make_mod_instruction(op_sizes size, const register_t& dest_reg, const register_t& dividend_reg, const register_t& divisor_reg);

    void make_div_instruction(op_sizes size, const register_t& dest_reg, const register_t& dividend_reg, const register_t& divisor_reg);

    void make_mul_instruction(op_sizes size, const register_t& dest_reg, const register_t& multiplicand_reg, const register_t& multiplier_reg);

    void make_load_instruction(op_sizes size, const register_t& dest_reg, const register_t& address_reg, int64_t offset);

    void make_store_instruction(op_sizes size, const register_t& address_reg, const register_t& src_reg, int64_t offset);

    void make_swap_instruction(op_sizes size, const register_t& dest_reg, const register_t& src_reg);

    void make_cmp_instruction(op_sizes size, const register_t& lhs_reg, const register_t& rhs_reg);

    void make_not_instruction(op_sizes size, const register_t& dest_reg, const register_t& src_reg);

    void make_neg_instruction(op_sizes size, const register_t& dest_reg, const register_t& src_reg);

    void make_move_instruction(op_sizes size, const register_t& dest_reg, uint64_t value);

    void make_move_instruction(op_sizes size, const register_t& dest_reg, double value);

    void make_move_instruction(op_sizes size, op_codes code, const register_t& dest_reg, const register_t& src_reg);

    void make_add_instruction(op_sizes size, const register_t& dest_reg, const register_t& augend_reg,
        const register_t& addend_reg);

    void make_sub_instruction(op_sizes size, const register_t& dest_reg, const register_t& minuend_reg,
        const register_t& subtrahend_reg);

    void make_sub_instruction_immediate(op_sizes size, const register_t& dest_reg, const register_t& minuend_reg,
        uint64_t subtrahend_immediate);

    void make_integer_constant_push_instruction(op_sizes size, uint64_t value);

    label* find_label_up(const std::string& label_name);

    label_ref_t* make_unresolved_label_ref(const std::string& label_name);

    void disassemble(instruction_block* block);
private:
    stack_frame_t stack_frame_;
    instruction_block* parent_ = nullptr;
    instruction_block_type_t type_;
    std::vector<block_entry_t> entries_ {};
    std::vector<instruction_block*> blocks_ {};
    listing_source_file_t* source_file_ = nullptr;
    std::unordered_map<std::string, label*> labels_ {};
    std::unordered_map<id_t, label_ref_t> unresolved_labels_ {};
    std::unordered_map<std::string, id_t> label_to_unresolved_ids_ {};
};
}

#endif // VM_INSTRUCTION_BLOCK_H_
