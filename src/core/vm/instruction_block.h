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
#include <stack>
namespace gfx {
enum class target_register_type_t {
    none,
    integer,
    floating_point,
};

struct target_register_t {
    target_register_type_t type;
    union {
        i_registers_t i;
        f_registers_t f;
    } reg;
};

enum class instruction_block_type_t {
    basic,
    procedure
};

struct instruction_comments_t {
    std::vector<std::string> lines {};
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

    void pop_target_register();

    // setxx
    void setz(i_registers_t dest_reg);

    void setnz(i_registers_t dest_reg);
    // branches
    void bne(const std::string& label_name);

    void beq(const std::string& label_name);

    void comment(const std::string& value);

    target_register_t* current_target_register();

    void push_target_register(i_registers_t reg);

    void push_target_register(f_registers_t reg);

    /// load variations
    template<class T>
    void load_to_ireg(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset = 0)
    {
        make_load_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, address_reg, offset);
    }

    /// store
    template<class T>
    void store_from_ireg(i_registers_t address_reg, i_registers_t src_reg, int64_t offset = 0)
    {
        make_store_instruction(TypeToOpSize::ToOpSize<T>(), address_reg, src_reg, offset);
    }

    // neg variations
    template<typename T>
    void neg(i_registers_t dest_reg, i_registers_t src_reg)
    {
        make_neg_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, src_reg);
    }

    // or variations
    void or_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_or_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    // xor variations
    void xor_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_xor_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    // and variations
    void and_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_and_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    // shl variations
    void shl_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_shl_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    // shr variations
    void shr_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_shr_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    // rol variations
    void rol_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_rol_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    // ror variations
    void ror_ireg_by_ireg(i_registers_t dest_reg, i_registers_t lhs_reg, i_registers_t rhs_reg)
    {
        make_ror_instruction(op_sizes::qword, dest_reg, lhs_reg, rhs_reg);
    }

    /// move
    template<class T>
    void move_to_ireg(i_registers_t dest_reg, T immediate)
    {
        make_move_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, immediate);
    }

    void move_ireg_to_ireg(i_registers_t dest_reg, i_registers_t src_reg);

    // cmp variations
    void cmp_u64(i_registers_t lhs_reg, i_registers_t rhs_reg);

    // inc variations
    template<typename T>
    void inc(i_registers_t reg)
    {
        make_inc_instruction(TypeToOpSize::ToOpSize<T>(), reg);
    }

    // not variations
    template<class T>
    void not_reg(i_registers_t dest_reg, i_registers_t src_reg)
    {
        make_not_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, src_reg);
    }

    // dec variations
    template<typename T>
    void dec(i_registers_t reg)
    {
        make_dec_instruction(TypeToOpSize::ToOpSize<T>(), reg);
    }

    // mul variations
    template<typename T>
    void mul_ireg_by_ireg(i_registers_t dest_reg, i_registers_t multiplicand_reg, i_registers_t multiplier_reg)
    {
        make_mul_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, multiplicand_reg, multiplier_reg);
    }

    // add variations
    template<typename T>
    void add_ireg_by_ireg(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg)
    {
        make_add_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, augend_reg, addened_reg);
    }

    // sub variations
    template<typename T>
    void sub_ireg_by_ireg(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg)
    {
        make_sub_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, augend_reg, addened_reg);
    }

    // div variations
    template<typename T>
    void div_ireg_by_ireg(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg)
    {
        make_div_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, dividend_reg, divisor_reg);
    }

    // mod variations
    template<typename T>
    void mod_ireg_by_ireg(i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg)
    {
        make_mod_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, dividend_reg, divisor_reg);
    }

    // swap variations
    template<typename T>
    void swap_ireg_with_ireg(i_registers_t dest_reg, i_registers_t src_reg)
    {
        make_swap_instruction(TypeToOpSize::ToOpSize<T>(), dest_reg, src_reg);
    }

    // test mask for zero and branch
    template<typename T>
    void test_mask_branch_if_zero(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg)
    {

    }

    // test mask for non-zero and branch
    void test_mask_branch_if_not_zero(i_registers_t value_reg, i_registers_t mask_reg, i_registers_t address_reg)
    {

    }

    ///
    template<class T, typename = std::enable_if<std::is_floating_point_v<T>>>
    void push_constant(T value)
    {
        make_float_constant_push_instruction(TypeToOpSize::ToOpSize<T>(), value);
    }

    template<class T>
    void push_constant(T value)
    {
        make_integer_constant_push_instruction(TypeToOpSize::ToOpSize<T>(), value);
    }

    template<typename T>
    void push(i_registers_t reg)
    {
        make_push_instruction(TypeToOpSize::ToOpSize<T>() , reg);
    }

    template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
    void push(f_registers_t reg)
    {
        make_push_instruction(TypeToOpSize::ToOpSize<T>() , reg);
    }

    // register allocators
    void free_ireg(i_registers_t reg);

    void free_freg(f_registers_t reg);

    template<class T>
    void pop(i_registers_t reg)
    {
        make_pop_instruction(TypeToOpSize::ToOpSize<T>(), reg);
    }

    template<class T, typename = std::enable_if<std::is_floating_point_v<T>>>
    void pop(f_registers_t reg)
    {
        make_pop_instruction(TypeToOpSize::ToOpSize<T>(), reg);
    }

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
    void make_shl_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t amount_reg);

    void make_rol_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t amount_reg);

    void make_shr_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t amount_reg);

    void make_ror_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t amount_reg);

    void make_and_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t mask_reg);

    void make_xor_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t mask_reg);

    void make_or_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t value_reg, i_registers_t mask_reg);

    void make_mod_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void make_div_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t dividend_reg, i_registers_t divisor_reg);

    void make_mul_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t multiplicand_reg, i_registers_t multiplier_reg);

    void make_load_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t address_reg, int64_t offset);

    void make_store_instruction(op_sizes size, i_registers_t address_reg, i_registers_t src_reg, int64_t offset);

    void make_swap_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void make_inc_instruction(op_sizes size, i_registers_t reg);

    void make_dec_instruction(op_sizes size, i_registers_t reg);

    void make_pop_instruction(op_sizes size, i_registers_t dest_reg);

    void make_pop_instruction(op_sizes size, f_registers_t dest_reg);

    void make_push_instruction(op_sizes size, i_registers_t reg);

    void make_push_instruction(op_sizes size, f_registers_t reg);

    void make_cmp_instruction(op_sizes size, i_registers_t lhs_reg, i_registers_t rhs_reg);

    void make_not_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void make_neg_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void make_move_instruction(op_sizes size, i_registers_t dest_reg, uint64_t value);

    void make_move_instruction(op_sizes size, f_registers_t dest_reg, double value);

    void make_move_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg);

    void make_add_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t augend_reg,
        i_registers_t addend_reg);

    void make_sub_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t minuend_reg,
        i_registers_t subtrahend_reg);

    void make_float_constant_push_instruction(op_sizes size, double value);

    void make_integer_constant_push_instruction(op_sizes size, uint64_t value);

    label_ref_t* find_unresolved_label_up(id_t id);

    label* find_label_up(const std::string& label_name);

    label_ref_t* make_unresolved_label_ref(const std::string& label_name);

    void disassemble(instruction_block* block);
private:
    instruction_block* parent_ = nullptr;
    instruction_block_type_t type_;
    std::vector<instruction_block*> blocks_ {};
    std::vector<instruction_t> instructions_ {};
    std::map<std::string, label*> labels_ {};
    std::set<f_registers_t> used_float_registers_ {};
    std::stack<target_register_t> target_registers_ {};
    std::set<i_registers_t> used_integer_registers_ {};
    std::map<size_t, instruction_comments_t> comments_ {};
    std::map<std::string, size_t> label_to_instruction_map_{};
    std::unordered_map<id_t, label_ref_t> unresolved_labels_ {};
    std::unordered_map<std::string, id_t> label_to_unresolved_ids_ {};
};
}

#endif // VM_INSTRUCTION_BLOCK_H_
