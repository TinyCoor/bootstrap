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
#include "stack_frame.h"
#include "assembly_listing.h"
namespace gfx {

enum class section_t : uint8_t {
    bss = 1,
    ro_data,
    data,
    text
};

inline static std::string_view section_name(section_t type) {
    switch (type) {
        case section_t::bss:    return "bss";
        case section_t::ro_data:return "ro_data";
        case section_t::data:   return "data";
        case section_t::text:   return "text";
    }
    return "unknown";
}

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

template <typename T>
struct register_allocator_t {
    register_allocator_t() {
        reset();
    }

    void reset() {
        used.clear();
        while (!available.empty()) {
            available.pop();
        }
        for (int8_t r = 63; r >=0; r--) {
            available.push(static_cast<T>(r));
        }
    }

    void free(T reg) {
        if (used.erase(reg) > 0) {
            available.push(reg);
        }
    }

    bool allocate(T& reg) {
        if (available.empty()) {
            return false;
        }
        reg = available.top();
        available.pop();
        used.insert(reg);
        return true;
    }

    std::set<T> used {};
    std::stack<T> available {};
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

    void disassemble(assembly_listing& listing);

    void clear_labels();

    void clear_blocks();

    void clear_instructions();

    target_register_t pop_target_register();

    // sections
    void section(section_t type);

    // data definitions
    void byte(uint8_t value);

    void word(uint16_t value);

    void dword(uint32_t value);

    void qword(uint64_t value);

    void reserve_byte(size_t count);

    void reserve_word(size_t count);

    void reserve_dword(size_t count);

    void reserve_qword(size_t count);

    void string(const std::string& value);

    void make_data_instruction(op_sizes size, uint64_t flags, uint64_t data);

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

    void sub_ireg_by_immediate(i_registers_t dest_reg, i_registers_t minuend_reg, uint64_t subtrahend_immediate);
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
    // alloc / free register
    bool allocate_reg(i_registers_t& reg);

    void free_reg(i_registers_t reg);

    bool allocate_reg(f_registers_t& reg);

    void free_reg(f_registers_t reg);

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

    stack_frame_t* stack_frame();
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

    void make_sub_instruction_immediate(op_sizes size, i_registers_t dest_reg, i_registers_t minuend_reg,
        uint64_t subtrahend_immediate);

    void make_float_constant_push_instruction(op_sizes size, double value);

    void make_integer_constant_push_instruction(op_sizes size, uint64_t value);

    label_ref_t* find_unresolved_label_up(id_t id);

    label* find_label_up(const std::string& label_name);

    label_ref_t* make_unresolved_label_ref(const std::string& label_name);

    void disassemble(assembly_listing& listing, instruction_block* block);
private:
    stack_frame_t stack_frame_;
    instruction_block* parent_ = nullptr;
    instruction_block_type_t type_;
    std::vector<instruction_block*> blocks_ {};
    std::vector<instruction_t> instructions_ {};
    std::map<std::string, label*> labels_ {};
    std::stack<target_register_t> target_registers_ {};
    std::map<size_t, instruction_comments_t> comments_ {};
    register_allocator_t<i_registers_t> i_register_allocator_ {};
    register_allocator_t<f_registers_t> f_register_allocator_ {};
    std::map<std::string, size_t> label_to_instruction_map_{};
    std::unordered_map<id_t, label_ref_t> unresolved_labels_ {};
    std::unordered_map<std::string, id_t> label_to_unresolved_ids_ {};
};
}

#endif // VM_INSTRUCTION_BLOCK_H_
