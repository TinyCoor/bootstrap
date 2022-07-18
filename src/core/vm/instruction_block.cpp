//
// Created by 12132 on 2022/7/17.
//

#include "instruction_block.h"
#include "fmt/color.h"
namespace gfx {

instruction_block::instruction_block(instruction_block* parent, instruction_block_type_t type)
    : parent_(parent), type_(type)
{
}

instruction_block::~instruction_block()
{
    clear_blocks();
    clear_labels();
    clear_instructions();
}
instruction_block_type_t instruction_block::type() const
{
    return type_;
}

void instruction_block::call(const std::string& proc_name)
{

}

label* instruction_block::make_label(const std::string& name)
{
    auto label = new class label(name);
    labels_.insert(std::make_pair(name, label));
    label_to_instruction_map_.insert(std::make_pair(name, instructions_.size()));
    return label;
}
void instruction_block::clear_labels()
{
    for (const auto& it : labels_) {
        delete it.second;
    }
    labels_.clear();
    label_to_instruction_map_.clear();
}

void instruction_block::clear_blocks()
{
    blocks_.clear();
}

void instruction_block::clear_instructions()
{
    instructions_.clear();
}

void instruction_block::add_block(instruction_block *block)
{
    blocks_.push_back(block);
}

void instruction_block::remove_block(instruction_block *block)
{
    auto it = std::find_if(blocks_.begin(), blocks_.end(),
        [&block](auto each) { return each == block; });
    if (it == blocks_.end()) {
        return;
    }
    blocks_.erase(it);
}

void instruction_block::rts()
{
    instruction_t rts_op;
    rts_op.op = op_codes::rts;
    instructions_.emplace_back(rts_op);
}

void instruction_block::dup()
{
    instruction_t dup_op;
    dup_op.op = op_codes::dup;
    instructions_.emplace_back(dup_op);
}

void instruction_block::nop()
{
    instruction_t no_op;
    no_op.op = op_codes::nop;
    instructions_.emplace_back(no_op);
}

void instruction_block::exit()
{
    instruction_t exit_op;
    exit_op.op = op_codes::exit;
    instructions_.emplace_back(exit_op);
}

void instruction_block::swi(uint8_t index)
{
    instruction_t swi_op;
    swi_op.op = op_codes::swi;
    swi_op.size = op_sizes::byte;
    swi_op.operands_count = 1u;
    swi_op.operands[0].type = operand_encoding_t::flags::integer;
    swi_op.operands[0].value.u64 = index;
    instructions_.emplace_back(swi_op);
}

void instruction_block::trap(uint8_t index)
{
    instruction_t trap_op;
    trap_op.op = op_codes::trap;
    trap_op.size = op_sizes::byte;
    trap_op.operands_count = 1;
    trap_op.operands[0].type = operand_encoding_t::flags::integer;
    trap_op.operands[0].value.u64 = index;
    instructions_.push_back(trap_op);
}

void instruction_block::move_f32_to_freg(f_registers_t dest_reg, float immediate)
{
    make_move_instruction(op_sizes::dword, dest_reg, immediate);
}

void instruction_block::move_f64_to_freg(f_registers_t dest_reg, double immediate)
{
    make_move_instruction(op_sizes::qword, dest_reg, immediate);
}

void instruction_block::move_u8_to_ireg(i_registers_t dest_reg, uint8_t immediate)
{
    make_move_instruction(op_sizes::byte, dest_reg, immediate);
}

void instruction_block::move_u16_to_ireg(i_registers_t dest_reg, uint16_t immediate)
{
    make_move_instruction(op_sizes::word, dest_reg, immediate);
}

void instruction_block::move_u32_to_ireg(i_registers_t dest_reg, uint32_t immediate)
{
    make_move_instruction(op_sizes::word, dest_reg, immediate);
}

void instruction_block::move_u64_to_ireg(i_registers_t dest_reg, uint64_t immediate)
{
    make_move_instruction(op_sizes::qword, dest_reg, immediate);
}

void instruction_block::move_ireg_to_ireg(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_move_instruction(op_sizes::qword, dest_reg, src_reg);
}

i_registers_t instruction_block::allocate_ireg()
{
    if (!used_integer_registers_.empty()) {
        auto reg = static_cast<i_registers_t>((*used_integer_registers_.rbegin()) + 1);
        used_integer_registers_.insert(reg);
        return reg;
    }
    used_integer_registers_.insert(i_registers_t::i0);
    return *used_integer_registers_.begin();
}
f_registers_t instruction_block::allocate_freg()
{
    if (!used_float_registers_.empty()) {
        auto reg = static_cast<f_registers_t>((*used_float_registers_.rbegin()) + 1);
        used_float_registers_.insert(reg);
        return reg;
    }
    used_float_registers_.insert(f_registers_t::f0);
    return *used_float_registers_.begin();
}

void instruction_block::push_u8(i_registers_t reg)
{
    make_push_instruction(op_sizes::byte, reg);
}
void instruction_block::push_u16(i_registers_t reg)
{
    make_push_instruction(op_sizes::byte, reg);
}
void instruction_block::push_f32(f_registers_t reg)
{
    make_push_instruction(op_sizes::byte, reg);
}
void instruction_block::push_u32(i_registers_t reg)
{
    make_push_instruction(op_sizes::word, reg);
}
void instruction_block::push_f64(f_registers_t reg)
{
    make_push_instruction(op_sizes::dword, reg);
}
void instruction_block::push_u64(i_registers_t reg)
{
    make_push_instruction(op_sizes::qword, reg);
}
void instruction_block::push_f32(float value)
{
    make_float_constant_push_instruction(op_sizes::dword, value);
}
void instruction_block::push_f64(double value)
{
    make_float_constant_push_instruction(op_sizes::qword, value);
}
void instruction_block::push_u8(uint8_t value)
{
    make_integer_constant_push_instruction(op_sizes::byte, value);
}
void instruction_block::push_u16(uint16_t value)
{
    make_integer_constant_push_instruction(op_sizes::word, value);
}
void instruction_block::push_u32(uint32_t value)
{
    make_integer_constant_push_instruction(op_sizes::qword, value);
}

void instruction_block::pop_f32(f_registers_t reg)
{
    make_pop_instruction(op_sizes::dword, reg);
}
void instruction_block::pop_f64(f_registers_t reg)
{
    make_pop_instruction(op_sizes::qword, reg);
}
void instruction_block::push_u64(uint64_t value)
{
    make_integer_constant_push_instruction(op_sizes::dword, value);
}
void instruction_block::pop_u8(i_registers_t reg)
{
    make_pop_instruction(op_sizes::byte, reg);
}

void instruction_block::pop_u16(i_registers_t reg)
{
    make_pop_instruction(op_sizes::word, reg);
}
void instruction_block::pop_u32(i_registers_t reg)
{
    make_pop_instruction(op_sizes::dword, reg);
}
void instruction_block::pop_u64(i_registers_t reg)
{
    make_pop_instruction(op_sizes::qword, reg);
}
bool instruction_block::reserve_ireg(i_registers_t reg)
{
    if (used_integer_registers_.count(reg) > 0) {
        return false;
    }
    used_integer_registers_.insert(reg);
    return true;
}
bool instruction_block::reserve_freg(f_registers_t reg)
{
    if (used_float_registers_.count(reg) > 0) {
        return false;
    }
    used_float_registers_.insert(reg);
    return true;
}
void instruction_block::jump_indirect(i_registers_t reg)
{
    instruction_t jmp_op;
    jmp_op.op = op_codes::jmp;
    jmp_op.size = op_sizes::qword;
    jmp_op.operands_count = 1;
    jmp_op.operands[0].type =
        operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    jmp_op.operands[0].value.r8 = reg;
    instructions_.push_back(jmp_op);
}

void instruction_block::jump_direct(const std::string &label_name)
{

}

void instruction_block::disassemble()
{
    disassemble(this);
}

void instruction_block::load_to_ireg_u8(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset)
{
    make_load_instruction(op_sizes::byte, dest_reg, address_reg, offset);
}
void instruction_block::load_to_ireg_u16(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset)
{
    make_load_instruction(op_sizes::word, dest_reg, address_reg, offset);
}
void instruction_block::load_to_ireg_u32(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset)
{
    make_load_instruction(op_sizes::dword, dest_reg, address_reg, offset);
}
void instruction_block::load_to_ireg_u64(i_registers_t dest_reg, i_registers_t address_reg, int64_t offset)
{
    make_load_instruction(op_sizes::qword, dest_reg, address_reg, offset);
}

void instruction_block::store_from_ireg_u8(i_registers_t src_reg, i_registers_t address_reg, int64_t offset)
{
    make_store_instruction(op_sizes::byte, src_reg, address_reg, offset);
}
void instruction_block::store_from_ireg_u16(i_registers_t src_reg, i_registers_t address_reg, int64_t offset)
{
    make_store_instruction(op_sizes::word, src_reg, address_reg, offset);
}
void instruction_block::store_from_ireg_u32(i_registers_t src_reg, i_registers_t address_reg, int64_t offset)
{
    make_store_instruction(op_sizes::dword, src_reg, address_reg, offset);
}
void instruction_block::store_from_ireg_u64(i_registers_t src_reg, i_registers_t address_reg, int64_t offset)
{
    make_store_instruction(op_sizes::qword, src_reg, address_reg, offset);
}
void instruction_block::inc_u8(i_registers_t reg)
{
    make_inc_instruction(op_sizes::byte, reg);
}
void instruction_block::inc_u16(i_registers_t reg)
{
    make_inc_instruction(op_sizes::word, reg);
}
void instruction_block::inc_u32(i_registers_t reg)
{
    make_inc_instruction(op_sizes::dword, reg);
}
void instruction_block::inc_u64(i_registers_t reg)
{
    make_inc_instruction(op_sizes::qword, reg);
}
void instruction_block::dec_u8(i_registers_t reg)
{
    make_dec_instruction(op_sizes::byte, reg);
}
void instruction_block::dec_u16(i_registers_t reg)
{
    make_dec_instruction(op_sizes::word, reg);
}
void instruction_block::dec_u32(i_registers_t reg)
{
    make_dec_instruction(op_sizes::dword, reg);
}
void instruction_block::dec_u64(i_registers_t reg)
{
    make_dec_instruction(op_sizes::qword, reg);
}
void instruction_block::mul_ireg_by_ireg_u8(i_registers_t dest_reg,
     i_registers_t multiplicand_reg, i_registers_t multiplier_reg)
{
}
void instruction_block::mul_ireg_by_ireg_u16(i_registers_t dest_reg,
    i_registers_t multiplicand_reg, i_registers_t multiplier_reg)
{

}
void instruction_block::mul_ireg_by_ireg_u32(i_registers_t dest_reg,
    i_registers_t multiplicand_reg, i_registers_t multiplier_reg)
{

}
void instruction_block::mul_ireg_by_ireg_u64(i_registers_t dest_reg,
    i_registers_t multiplicand_reg, i_registers_t multiplier_reg)
{

}
void instruction_block::add_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_add_instruction(op_sizes::byte, dest_reg ,augend_reg, addened_reg);
}
void instruction_block::add_ireg_by_ireg_u16(i_registers_t dest_reg,
    i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_add_instruction(op_sizes::word, dest_reg ,augend_reg, addened_reg);

}
void instruction_block::add_ireg_by_ireg_u32(i_registers_t dest_reg,
    i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_add_instruction(op_sizes::dword, dest_reg ,augend_reg, addened_reg);

}
void instruction_block::add_ireg_by_ireg_u64(i_registers_t dest_reg,
    i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_add_instruction(op_sizes::qword, dest_reg ,augend_reg, addened_reg);
}
void instruction_block::sub_ireg_by_ireg_u8(i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_sub_instruction(op_sizes::byte, dest_reg ,augend_reg, addened_reg);
}
void instruction_block::sub_ireg_by_ireg_u16(i_registers_t dest_reg,
    i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_sub_instruction(op_sizes::word, dest_reg ,augend_reg, addened_reg);
}
void instruction_block::sub_ireg_by_ireg_u32(i_registers_t dest_reg,
    i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_sub_instruction(op_sizes::dword, dest_reg ,augend_reg, addened_reg);
}
void instruction_block::sub_ireg_by_ireg_u64(i_registers_t dest_reg,
    i_registers_t augend_reg, i_registers_t addened_reg)
{
    make_sub_instruction(op_sizes::qword, dest_reg ,augend_reg, addened_reg);
}
void instruction_block::div_ireg_by_ireg_u8(i_registers_t dest_reg,
    i_registers_t dividend_reg, i_registers_t divisor_reg)
{

}
void instruction_block::div_ireg_by_ireg_u16(i_registers_t dest_reg,
                                             i_registers_t dividend_reg,
                                             i_registers_t divisor_reg)
{

}
void instruction_block::div_ireg_by_ireg_u32(i_registers_t dest_reg,
    i_registers_t dividend_reg, i_registers_t divisor_reg)
{

}
void instruction_block::div_ireg_by_ireg_u64(i_registers_t dest_reg,
                                             i_registers_t dividend_reg,
                                             i_registers_t divisor_reg)
{

}
void instruction_block::mod_ireg_by_ireg_u8(i_registers_t dest_reg,
                                            i_registers_t dividend_reg,
                                            i_registers_t divisor_reg)
{

}
void instruction_block::mod_ireg_by_ireg_u16(i_registers_t dest_reg,
                                             i_registers_t dividend_reg,
                                             i_registers_t divisor_reg)
{

}
void instruction_block::mod_ireg_by_ireg_u32(i_registers_t dest_reg,
                                             i_registers_t dividend_reg,
                                             i_registers_t divisor_reg)
{

}
void instruction_block::mod_ireg_by_ireg_u64(i_registers_t dest_reg,
                                             i_registers_t dividend_reg,
                                             i_registers_t divisor_reg)
{

}
void instruction_block::swap_ireg_with_ireg_u8(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_swap_instruction(op_sizes::byte, dest_reg, src_reg);
}
void instruction_block::swap_ireg_with_ireg_u16(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_swap_instruction(op_sizes::word, dest_reg, src_reg);
}
void instruction_block::swap_ireg_with_ireg_u32(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_swap_instruction(op_sizes::dword, dest_reg, src_reg);
}
void instruction_block::swap_ireg_with_ireg_u64(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_swap_instruction(op_sizes::qword, dest_reg, src_reg);
}
void instruction_block::test_mask_branch_if_zero_u8(i_registers_t value_reg,
                                                    i_registers_t mask_reg,
                                                    i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_zero_u16(i_registers_t value_reg,
                                                     i_registers_t mask_reg,
                                                     i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_zero_u32(i_registers_t value_reg,
                                                     i_registers_t mask_reg,
                                                     i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_zero_u64(i_registers_t value_reg,
                                                     i_registers_t mask_reg,
                                                     i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_not_zero_u8(i_registers_t value_reg,
                                                        i_registers_t mask_reg,
                                                        i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_not_zero_u16(i_registers_t value_reg,
                                                         i_registers_t mask_reg,
                                                         i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_not_zero_u32(i_registers_t value_reg,
                                                         i_registers_t mask_reg,
                                                         i_registers_t address_reg)
{

}
void instruction_block::test_mask_branch_if_not_zero_u64(i_registers_t value_reg,
                                                         i_registers_t mask_reg,
                                                         i_registers_t address_reg)
{

}

void instruction_block::free_ireg(i_registers_t reg)
{

}
void instruction_block::free_freg(f_registers_t reg)
{

}
void instruction_block::call_foreign(const std::string &proc_name)
{
    auto label_ref = make_unresolved_label_ref(proc_name);
    instruction_t ffi_op;
    ffi_op.op = op_codes::ffi;
    ffi_op.size = op_sizes::qword;
    ffi_op.operands_count = 1;
    ffi_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    ffi_op.operands[0].value.u64 = label_ref->id;
    instructions_.push_back(ffi_op);
}
void instruction_block::make_inc_instruction(op_sizes size, i_registers_t reg)
{
    instruction_t inc_op;
    inc_op.op = op_codes::inc;
    inc_op.size = size;
    inc_op.operands_count = 1;
    inc_op.operands[0].type =
        operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    inc_op.operands[0].value.r8 = reg;
    instructions_.push_back(inc_op);
}
void instruction_block::make_dec_instruction(op_sizes size, i_registers_t reg)
{
    instruction_t dec_op;
    dec_op.op = op_codes::dec;
    dec_op.operands_count = 1;
    dec_op.size = size;
    dec_op.operands[0].value.r8 = reg;
    dec_op.operands[0].type =operand_encoding_t::integer | operand_encoding_t::reg;
    instructions_.emplace_back(dec_op);
}
void instruction_block::make_load_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t address_reg, int64_t offset)
{
    instruction_t load_op;
    load_op.op = op_codes::load;
    load_op.size = size;
    load_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    load_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    load_op.operands[0].value.r8 = dest_reg;
    load_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    load_op.operands[1].value.r8 = address_reg;
    if (load_op.operands_count == 3) {
        load_op.operands[2].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::constant;
        if (offset < 0) {
            load_op.operands[2].type |= operand_encoding_t::flags::negative;
        }
        load_op.operands[2].value.u64 = static_cast<uint64_t>(offset);
    }
    instructions_.push_back(load_op);
}

void instruction_block::make_store_instruction(op_sizes size, i_registers_t src_reg,
    i_registers_t address_reg, int64_t offset)
{
    instruction_t store_op;
    store_op.op = op_codes::store;
    store_op.size = size;
    store_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    store_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    store_op.operands[0].value.r8 = src_reg;
    store_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    store_op.operands[1].value.r8 = address_reg;

    if (store_op.operands_count == 3) {
        store_op.operands[2].type = operand_encoding_t::flags::integer
                | operand_encoding_t::flags::constant;
        if (offset < 0) {
            store_op.operands[2].type |= operand_encoding_t::flags::negative;
        }
        store_op.operands[2].value.u64 = static_cast<uint64_t>(offset);
    }
    instructions_.push_back(store_op);
}
void instruction_block::make_swap_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg)
{
    instruction_t swap_op;
    swap_op.op = op_codes::swap;
    swap_op.size = size;
    swap_op.operands_count = 2;
    swap_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    swap_op.operands[0].value.r8 = dest_reg;
    swap_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    swap_op.operands[1].value.r8 = src_reg;
    instructions_.push_back(swap_op);
}
void instruction_block::make_pop_instruction(op_sizes size, i_registers_t dest_reg)
{
    instruction_t pop_op;
    pop_op.op = op_codes::pop;
    pop_op.size = size;
    pop_op.operands_count = 1;
    pop_op.operands[0].type =
        operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    pop_op.operands[0].value.r8 = dest_reg;
    instructions_.push_back(pop_op);
}
void instruction_block::make_pop_instruction(op_sizes size, f_registers_t dest_reg)
{
    instruction_t pop_op;
    pop_op.op = op_codes::pop;
    pop_op.size = size;
    pop_op.operands_count = 1;
    pop_op.operands[0].type = operand_encoding_t::flags::reg;
    pop_op.operands[0].value.r8 = dest_reg;
    instructions_.push_back(pop_op);
}
void instruction_block::make_move_instruction(op_sizes size, i_registers_t dest_reg, uint64_t value)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant;
    move_op.operands[0].value.u64 = value;
    move_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    move_op.operands[1].value.r8 = dest_reg;
    instructions_.push_back(move_op);
}
void instruction_block::make_move_instruction(op_sizes size, f_registers_t dest_reg, double value)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::constant;
    move_op.operands[0].value.d64 = value;
    move_op.operands[1].type = operand_encoding_t::flags::reg;
    move_op.operands[1].value.r8 = dest_reg;
    instructions_.push_back(move_op);
}
void instruction_block::make_move_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r8 = src_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    move_op.operands[1].value.r8 = dest_reg;
    instructions_.push_back(move_op);
}
void instruction_block::disassemble(instruction_block *block)
{
    for (const auto& it : block->labels_) {
        fmt::print("{}:\n", it.first);
    }
    for (const auto& inst : block->instructions_) {
        auto stream = inst.disassemble([&](uint64_t id) -> std::string {
          auto label_ref = block->find_unresolved_label_up(static_cast<id_t>(id));
          return label_ref != nullptr ? label_ref->name : fmt::format("unresolved_ref_id({})", id);
        });
        fmt::print("\t{}\n", stream);
    }
    for (auto child_block : block->blocks_) {
        disassemble(child_block);
    }
}

void instruction_block::make_float_constant_push_instruction(op_sizes size, double value)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant;
    push_op.operands[0].value.d64 = value;
    instructions_.push_back(push_op);
}

void instruction_block::make_integer_constant_push_instruction(op_sizes size, uint64_t value)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant;
    push_op.operands[0].value.u64 = value;
    instructions_.push_back(push_op);
}

label_ref_t *instruction_block::find_unresolved_label_up(id_t id)
{
    auto current_block = this;
    while (current_block != nullptr) {
        auto it = current_block->unresolved_labels_.find(id);
        if (it != current_block->unresolved_labels_.end()) {
            return &it->second;
        }
        current_block = current_block->parent_;
    }
    return nullptr;
}

label *instruction_block::find_label_up(const std::string &label_name)
{
    auto current_block = this;
    while (current_block != nullptr) {
        auto it = current_block->labels_.find(label_name);
        if (it != current_block->labels_.end()) {
            return it->second;
        }
        current_block = current_block->parent_;
    }
    return nullptr;
}

void instruction_block::make_push_instruction(op_sizes size, i_registers_t reg)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    push_op.operands[0].value.r8 = reg;
    instructions_.push_back(push_op);
}

void instruction_block::make_push_instruction(op_sizes size, f_registers_t reg)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::reg;
    push_op.operands[0].value.r8 = reg;
    instructions_.push_back(push_op);
}

label_ref_t *instruction_block::make_unresolved_label_ref(const std::string &label_name)
{
    auto it = label_to_unresolved_ids_.find(label_name);
    if (it != label_to_unresolved_ids_.end()) {
        auto ref_it = unresolved_labels_.find(it->second);
        if (ref_it != unresolved_labels_.end()) {
            return &ref_it->second;
        }
    }

    auto label = find_label_up(label_name);
    auto ref_id = id_pool::instance()->allocate();
    auto insert_pair = unresolved_labels_.insert(std::make_pair(ref_id,
        label_ref_t {
            .id = ref_id,
            .name = label_name,
            .resolved = label
        }));
    label_to_unresolved_ids_.insert(std::make_pair(label_name, ref_id));

    return &insert_pair.first.operator->()->second;
}

void instruction_block::move_label_to_ireg(i_registers_t dest_reg, const std::string &label_name)
{
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = op_sizes::qword;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    move_op.operands[0].value.u64 = label_ref->id;
    move_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    move_op.operands[1].value.r8 = dest_reg;
    instructions_.push_back(move_op);
}

instruction_block *instruction_block::parent()
{
    return parent_;
}

void instruction_block::neg_u8(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_neg_instruction(op_sizes::byte, dest_reg, src_reg);
}

void instruction_block::neg_u16(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_neg_instruction(op_sizes::word, dest_reg, src_reg);
}
void instruction_block::neg_u32(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_neg_instruction(op_sizes::dword, dest_reg, src_reg);
}
void instruction_block::neg_u64(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_neg_instruction(op_sizes::qword, dest_reg, src_reg);
}

void instruction_block::make_neg_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg)
{
    instruction_t neg_op;
    neg_op.op = op_codes::neg;
    neg_op.size = size;
    neg_op.operands_count = 2;
    neg_op.operands[0].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    neg_op.operands[0].value.r8 = dest_reg;
    neg_op.operands[1].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    neg_op.operands[1].value.r8 = src_reg;
    instructions_.push_back(neg_op);
}

void instruction_block::make_add_instruction(op_sizes size,
    i_registers_t dest_reg, i_registers_t augend_reg, i_registers_t addend_reg)
{
    instruction_t add_op;
    add_op.op = op_codes::add;
    add_op.size = size;
    add_op.operands_count = 3;
    add_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    add_op.operands[0].value.r8 = dest_reg;
    add_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    add_op.operands[1].value.r8 = augend_reg;
    add_op.operands[2].type =
        operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    add_op.operands[2].value.r8 = addend_reg;
    instructions_.push_back(add_op);
}
void instruction_block::make_sub_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t minuend_reg, i_registers_t subtrahend_reg)
{
    instruction_t sub_op;
    sub_op.op = op_codes::sub;
    sub_op.size = size;
    sub_op.operands_count = 3;
    sub_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    sub_op.operands[0].value.r8 = dest_reg;
    sub_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    sub_op.operands[1].value.r8 = minuend_reg;
    sub_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    sub_op.operands[2].value.r8 = subtrahend_reg;
    instructions_.push_back(sub_op);
}

}