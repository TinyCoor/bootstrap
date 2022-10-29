//
// Created by 12132 on 2022/7/17.
//

#include <sstream>
#include "instruction_block.h"
#include "fmt/color.h"
namespace gfx {

instruction_block::instruction_block(instruction_block_type_t type)
    : id_(id_pool::instance()->allocate()), type_(type)
{
}

instruction_block::~instruction_block()
{
    clear_entries();
}

id_t instruction_block::id() const
{
    return id_;
}


instruction_block_type_t instruction_block::type() const
{
    return type_;
}

void instruction_block::call(const label_ref_t* label_ref)
{
    instruction_t jsr_op;
    jsr_op.op = op_codes::jsr;
    jsr_op.size = op_sizes::qword;
    jsr_op.operands_count = 1;
    jsr_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    jsr_op.operands[0].value.u = label_ref->id;
    make_block_entry(jsr_op);

}

void instruction_block::rts()
{
    instruction_t rts_op;
    rts_op.op = op_codes::rts;
    make_block_entry(rts_op);
}

[[maybe_unused]] void instruction_block::dup()
{
    instruction_t dup_op;
    dup_op.op = op_codes::dup;
    make_block_entry(dup_op);
}

void instruction_block::nop()
{
    instruction_t no_op;
    no_op.op = op_codes::nop;
    make_block_entry(no_op);
}

void instruction_block::exit()
{
    instruction_t exit_op;
    exit_op.op = op_codes::exit;
    make_block_entry(exit_op);
}

void instruction_block::swi(uint8_t index)
{
    instruction_t swi_op;
    swi_op.op = op_codes::swi;
    swi_op.size = op_sizes::byte;
    swi_op.operands_count = 1u;
    swi_op.operands[0].type = operand_encoding_t::flags::integer;
    swi_op.operands[0].value.u = index;
    make_block_entry(swi_op);
}

void instruction_block::trap(uint8_t index)
{
    instruction_t trap_op;
    trap_op.op = op_codes::trap;
    trap_op.size = op_sizes::byte;
    trap_op.operands_count = 1;
    trap_op.operands[0].type = operand_encoding_t::flags::integer;
    trap_op.operands[0].value.u = index;
    make_block_entry(trap_op);
}

void instruction_block::jump_indirect(const register_t& reg)
{
    instruction_t jmp_op;
    jmp_op.op = op_codes::jmp;
    jmp_op.size = op_sizes::qword;
    jmp_op.operands_count = 1;
    jmp_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    jmp_op.operands[0].value.r = reg.number;
    make_block_entry(jmp_op);
}

void instruction_block::jump_direct(const label_ref_t *label_ref)
{
    instruction_t jmp_op;
    jmp_op.op = op_codes::jmp;
    jmp_op.size = op_sizes::qword;
    jmp_op.operands_count = 1;
    jmp_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    jmp_op.operands[0].value.u = label_ref->id;
    make_block_entry(jmp_op);
}

void instruction_block::call_foreign(uint64_t proc_address)
{
    instruction_t ffi_op;
    ffi_op.op = op_codes::ffi;
    ffi_op.size = op_sizes::qword;
    ffi_op.operands_count = 1;
    ffi_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant;
    ffi_op.operands[0].value.u = proc_address;
    make_block_entry(ffi_op);
}

void instruction_block::make_inc_instruction(op_sizes size, const register_t& reg)
{
    instruction_t inc_op;
    inc_op.op = op_codes::inc;
    inc_op.size = size;
    inc_op.operands_count = 1;
    inc_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    inc_op.operands[0].value.r = reg.number;
    make_block_entry(inc_op);
}

void instruction_block::make_dec_instruction(op_sizes size, const register_t& reg)
{
    instruction_t dec_op;
    dec_op.op = op_codes::dec;
    dec_op.operands_count = 1;
    dec_op.size = size;
    dec_op.operands[0].value.r = reg.number;
    dec_op.operands[0].type =operand_encoding_t::integer | operand_encoding_t::reg;
    make_block_entry(dec_op);
}

void instruction_block::make_load_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& address_reg, int64_t offset)
{
    instruction_t load_op;
    load_op.op = op_codes::load;
    load_op.size = size;
    load_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    load_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type == register_type_t::integer) {
        load_op.operands[0].type |= operand_encoding_t::flags::integer;
    }
    load_op.operands[0].value.r = dest_reg.number;
    load_op.operands[1].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::reg;
    load_op.operands[1].value.r = address_reg.number;
    if (load_op.operands_count == 3) {
        load_op.operands[2].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::constant;
        if (offset < 0) {
            load_op.operands[2].type |= operand_encoding_t::flags::negative;
        }
        load_op.operands[2].value.u = static_cast<uint64_t>(offset);
    }
    make_block_entry(load_op);
}

void instruction_block::make_store_instruction(op_sizes size, const register_t& address_reg,
    const register_t& src_reg, int64_t offset)
{
    instruction_t store_op;
    store_op.op = op_codes::store;
    store_op.size = size;
    store_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    store_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    store_op.operands[0].value.r = address_reg.number;

    store_op.operands[1].type = operand_encoding_t::flags::reg;
    if (src_reg.type == register_type_t::integer) {
        store_op.operands[1].type |= operand_encoding_t::flags::integer;
    }
    store_op.operands[1].value.r = src_reg.number;
    if (store_op.operands_count == 3) {
        store_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant;
        if (offset < 0) {
            store_op.operands[2].type |= operand_encoding_t::flags::negative;
        }
        store_op.operands[2].value.u = static_cast<uint64_t>(offset);
    }
    make_block_entry(store_op);
}

void instruction_block::make_swap_instruction(op_sizes size, const register_t& dest_reg, const register_t& src_reg)
{
    instruction_t swap_op;
    swap_op.op = op_codes::swap;
    swap_op.size = size;
    swap_op.operands_count = 2;
    swap_op.operands[0].type = operand_encoding_t::flags::integer
       | operand_encoding_t::flags::reg;
    swap_op.operands[0].value.r = dest_reg.number;
    swap_op.operands[1].type = operand_encoding_t::flags::integer
       | operand_encoding_t::flags::reg;
    swap_op.operands[1].value.r = src_reg.number;
    make_block_entry(swap_op);
}

void instruction_block::make_pop_instruction(const register_t& dest_reg)
{
    instruction_t pop_op;
    pop_op.op = op_codes::pop;
    pop_op.size = dest_reg.size;
    pop_op.operands_count = 1;
    pop_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type == register_type_t::integer) {
        pop_op.operands[0].type |= operand_encoding_t::flags::integer;
    }
    pop_op.operands[0].value.r = dest_reg.number;
    make_block_entry(pop_op);
}

void instruction_block::make_move_instruction(op_sizes size, const register_t& dest_reg, uint64_t value)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg.number;
    move_op.operands[1].type = operand_encoding_t::flags::integer |
        operand_encoding_t::flags::constant;
    move_op.operands[1].value.u = value;
    make_block_entry(move_op);
}

void instruction_block::make_move_instruction(op_sizes size, const register_t& dest_reg, double value)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg.number;
    move_op.operands[1].type = operand_encoding_t::flags::constant;
    move_op.operands[1].value.d = value;
    make_block_entry(move_op);
}

void instruction_block::make_move_instruction(op_sizes size, op_codes code, const register_t& dest_reg, const register_t& src_reg)
{
    instruction_t move_op;
    move_op.op = code;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg.number;
    move_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[1].value.r = src_reg.number;
    make_block_entry(move_op);
}

void instruction_block::make_integer_constant_push_instruction(op_sizes size, uint64_t value)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant;
    push_op.operands[0].value.u = value;
    make_block_entry(push_op);
}

void instruction_block::make_push_instruction(const register_t& dest_reg)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = dest_reg.size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type== register_type_t::integer) {
        push_op.operands[0].type |= operand_encoding_t::flags::integer;
    }
    push_op.operands[0].value.r = dest_reg.number;
    make_block_entry(push_op);
}


void instruction_block::move_label_to_reg(const register_t& dest_reg, const label_ref_t *label_ref)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = op_sizes::qword;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg.number;
    move_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    move_op.operands[1].value.u = label_ref->id;
    make_block_entry(move_op);
}

void instruction_block::make_neg_instruction(op_sizes size, const register_t& dest_reg, const register_t& src_reg)
{
    instruction_t neg_op;
    neg_op.op = op_codes::neg;
    neg_op.size = size;
    neg_op.operands_count = 2;
    neg_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type == register_type_t::integer) {
        neg_op.operands[0].type |= operand_encoding_t::flags::integer;
    }
    neg_op.operands[0].value.r = dest_reg.number;
    neg_op.operands[1].type = operand_encoding_t::flags::reg;
    if (src_reg.type == register_type_t::integer) {
        neg_op.operands[1].type |= operand_encoding_t::flags::integer;
    }
    neg_op.operands[1].value.r = src_reg.number;
    make_block_entry(neg_op);
}

void instruction_block::make_add_instruction(op_sizes size,
    const register_t& dest_reg, const register_t& augend_reg, const register_t& addend_reg)
{
    instruction_t add_op;
    add_op.op = op_codes::add;
    add_op.size = size;
    add_op.operands_count = 3;
    add_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    add_op.operands[0].value.r = dest_reg.number;
    add_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    add_op.operands[1].value.r = augend_reg.number;
    add_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    add_op.operands[2].value.r = addend_reg.number;
    make_block_entry(add_op);
}

void instruction_block::make_sub_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& minuend_reg, const register_t& subtrahend_reg)
{
    instruction_t sub_op;
    sub_op.op = op_codes::sub;
    sub_op.size = size;
    sub_op.operands_count = 3;
    sub_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[0].value.r = dest_reg.number;
    sub_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[1].value.r = minuend_reg.number;
    sub_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[2].value.r = subtrahend_reg.number;
    make_block_entry(sub_op);
}

void instruction_block::move_reg_to_reg(const register_t& dest_reg, const register_t& src_reg)
{
    make_move_instruction(op_sizes::qword, op_codes::move, dest_reg, src_reg);
}

void instruction_block::make_cmp_instruction(op_sizes size, const register_t& lhs_reg, const register_t& rhs_reg)
{
    instruction_t cmp_op;
    cmp_op.op = op_codes::cmp;
    cmp_op.size = size;
    cmp_op.operands_count = 2;
    cmp_op.operands[0].type = operand_encoding_t::flags::reg;
    cmp_op.operands[0].value.r = lhs_reg.number;
    cmp_op.operands[1].type = operand_encoding_t::flags::reg;
    cmp_op.operands[1].value.r = rhs_reg.number;
    if (lhs_reg.type == register_type_t::integer && rhs_reg.type == register_type_t::integer) {
        cmp_op.operands[0].type |= operand_encoding_t::flags::integer;
        cmp_op.operands[1].type |= operand_encoding_t::flags::integer;
    } else {

    }
    make_block_entry(cmp_op);
}

void instruction_block::make_not_instruction(op_sizes size, const register_t& dest_reg, const register_t& src_reg)
{
    instruction_t not_op;
    not_op.op = op_codes::not_op;
    not_op.size = size;
    not_op.operands_count = 2;
    not_op.operands[0].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    not_op.operands[0].value.r = dest_reg.number;
    not_op.operands[1].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    not_op.operands[1].value.r = src_reg.number;
    make_block_entry(not_op);
}


void instruction_block::beq(const label_ref_t *label_ref)
{
    instruction_t branch_op;
    branch_op.op = op_codes::beq;
    branch_op.size = op_sizes::qword;
    branch_op.operands_count = 1;
    branch_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    branch_op.operands[0].value.u = label_ref->id;
    make_block_entry(branch_op);
}

void instruction_block::cmp(const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_cmp_instruction(lhs_reg.size, lhs_reg, rhs_reg);
}

void instruction_block::bne(const label_ref_t *label_ref)
{
    instruction_t branch_op;
    branch_op.op = op_codes::bne;
    branch_op.size = op_sizes::qword;
    branch_op.operands_count = 1;
    branch_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    branch_op.operands[0].value.u = label_ref->id;
    make_block_entry(branch_op);
}

void instruction_block::make_shl_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& amount_reg)
{
    instruction_t shift_op;
    shift_op.op = op_codes::shl;
    shift_op.size = size;
    shift_op.operands_count = 3;
    shift_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[0].value.r = dest_reg.number;
    shift_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[1].value.r = value_reg.number;
    shift_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[2].value.r = amount_reg.number;
    make_block_entry(shift_op);
}

void instruction_block::make_rol_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& amount_reg)
{
    instruction_t rotate_op;
    rotate_op.op = op_codes::rol;
    rotate_op.size = size;
    rotate_op.operands_count = 3;
    rotate_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[0].value.r = dest_reg.number;
    rotate_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[1].value.r = value_reg.number;
    rotate_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[2].value.r = amount_reg.number;
    make_block_entry(rotate_op);
}

void instruction_block::make_shr_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& amount_reg)
{
    instruction_t shift_op;
    shift_op.op = op_codes::shr;
    shift_op.size = size;
    shift_op.operands_count = 3;
    shift_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[0].value.r = dest_reg.number;
    shift_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[1].value.r = value_reg.number;
    shift_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[2].value.r = amount_reg.number;
    make_block_entry(shift_op);
}

void instruction_block::make_ror_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& amount_reg)
{
    instruction_t rotate_op;
    rotate_op.op = op_codes::ror;
    rotate_op.size = size;
    rotate_op.operands_count = 3;
    rotate_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[0].value.r = dest_reg.number;
    rotate_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[1].value.r = value_reg.number;
    rotate_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[2].value.r = amount_reg.number;
    make_block_entry(rotate_op);
}

void instruction_block::make_and_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& mask_reg)
{
    instruction_t and_op;
    and_op.op = op_codes::and_op;
    and_op.size = size;
    and_op.operands_count = 3;
    and_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    and_op.operands[0].value.r = dest_reg.number;
    and_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    and_op.operands[1].value.r = value_reg.number;
    and_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    and_op.operands[2].value.r = mask_reg.number;
    make_block_entry(and_op);
}

void instruction_block::make_xor_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& mask_reg)
{
    instruction_t xor_op;
    xor_op.op = op_codes::xor_op;
    xor_op.size = size;
    xor_op.operands_count = 3;
    xor_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    xor_op.operands[0].value.r = dest_reg.number;
    xor_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    xor_op.operands[1].value.r = value_reg.number;
    xor_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    xor_op.operands[2].value.r = mask_reg.number;
    make_block_entry(xor_op);
}

void instruction_block::make_or_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& value_reg, const register_t& mask_reg)
{
    instruction_t or_op;
    or_op.op = op_codes::or_op;
    or_op.size = size;
    or_op.operands_count = 3;
    or_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    or_op.operands[0].value.r = dest_reg.number;
    or_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    or_op.operands[1].value.r = value_reg.number;
    or_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    or_op.operands[2].value.r = mask_reg.number;
    make_block_entry(or_op);
}

void instruction_block::make_mod_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& dividend_reg, const register_t& divisor_reg)
{
    instruction_t mod_op;
    mod_op.op = op_codes::mod;
    mod_op.size = size;
    mod_op.operands_count = 3;
    mod_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mod_op.operands[0].value.r = dest_reg.number;
    mod_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mod_op.operands[1].value.r = dividend_reg.number;
    mod_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mod_op.operands[2].value.r = divisor_reg.number;
    make_block_entry(mod_op);
}

void instruction_block::make_div_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& dividend_reg, const register_t& divisor_reg)
{
    instruction_t div_op;
    div_op.op = op_codes::div;
    div_op.size = size;
    div_op.operands_count = 3;
    div_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type == register_type_t::integer) {
        div_op.operands[0].type |= operand_encoding_t::flags::integer;
    }
    div_op.operands[0].value.r = dest_reg.number;
    div_op.operands[1].type = operand_encoding_t::flags::reg;
    if (dividend_reg.type == register_type_t::integer) {
        div_op.operands[1].type |= operand_encoding_t::flags::integer;
    }
    div_op.operands[1].value.r = dividend_reg.number;
    div_op.operands[2].type = operand_encoding_t::flags::reg;
    if (divisor_reg.type == register_type_t::integer) {
        div_op.operands[2].type |= operand_encoding_t::flags::integer;
    }
    div_op.operands[2].value.r = divisor_reg.number;
    make_block_entry(div_op);
}

void instruction_block::make_mul_instruction(op_sizes size, const register_t& dest_reg,
    const register_t& multiplicand_reg, const register_t& multiplier_reg)
{
    instruction_t mul_op;
    mul_op.op = op_codes::mul;
    mul_op.size = size;
    mul_op.operands_count = 3;
    mul_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type == register_type_t::integer) {
        mul_op.operands[0].type |= operand_encoding_t::integer;
    }
    mul_op.operands[0].value.r = dest_reg.number;
    mul_op.operands[1].type = operand_encoding_t::flags::reg;
    if (multiplicand_reg.type == register_type_t::integer) {
        mul_op.operands[1].type |= operand_encoding_t::integer;
    }
    mul_op.operands[1].value.r = multiplicand_reg.number;
    mul_op.operands[2].type = operand_encoding_t::flags::reg;
    if (multiplier_reg.type == register_type_t::integer) {
        mul_op.operands[2].type |= operand_encoding_t::integer;
    }
    mul_op.operands[2].value.r = multiplier_reg.number;
    make_block_entry(mul_op);
}

void instruction_block::setz(const register_t& dest_reg)
{
    instruction_t setz_op;
    setz_op.op = op_codes::setz;
    setz_op.size = op_sizes::qword;
    setz_op.operands_count = 1;
    setz_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    setz_op.operands[0].value.r = dest_reg.number;
    make_block_entry(setz_op);
}

void instruction_block::setnz(const register_t& dest_reg)
{
    instruction_t setnz_op;
    setnz_op.op = op_codes::setnz;
    setnz_op.size = op_sizes::qword;
    setnz_op.operands_count = 1;
    setnz_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    setnz_op.operands[0].value.r = dest_reg.number;
    make_block_entry(setnz_op);
}

void instruction_block::sub_reg_by_immediate(const register_t& dest_reg, const register_t& minuend_reg,
    uint64_t subtrahend_immediate)
{
    make_sub_instruction_immediate(dest_reg.size, dest_reg, minuend_reg, subtrahend_immediate);
}

void instruction_block::make_sub_instruction_immediate(op_sizes size, const register_t& dest_reg,
    const register_t& minuend_reg, uint64_t subtrahend_immediate)
{
    instruction_t sub_op;
    sub_op.op = op_codes::sub;
    sub_op.size = size;
    sub_op.operands_count = 3;
    sub_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[0].value.r = dest_reg.number;
    sub_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[1].value.r = minuend_reg.number;
    sub_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant;
    sub_op.operands[2].value.u = subtrahend_immediate;
    make_block_entry(sub_op);
}

void instruction_block::section(section_t type)
{
    make_block_entry(type);
}

void instruction_block::bytes(const std::vector<uint8_t> &values)
{
    data_definition_t def {
        .size = op_sizes::byte,
        .type = data_definition_type_t::initialized,
    };
    for (const auto& v : values) {
        def.values.emplace_back(v);
    }
    make_block_entry(def);
}

void instruction_block::words(const std::vector<uint16_t> &values)
{
    data_definition_t def {
        .size = op_sizes::word,
        .type = data_definition_type_t::initialized,
    };
    for (const auto& v : values) {
        def.values.emplace_back(v);
    }
    make_block_entry(def);
}

void instruction_block::dwords(const std::vector<uint32_t> &values)
{
    data_definition_t def {
        .size = op_sizes::dword,
        .type = data_definition_type_t::initialized,
    };
    for (const auto& v : values) {
        def.values.emplace_back(v);
    }
    make_block_entry(def);
}

void instruction_block::qwords(const std::vector<uint64_t> &values)
{
    data_definition_t def {
        .size = op_sizes::qword,
        .type = data_definition_type_t::initialized,
    };
    for (const auto& v : values) {
        def.values.emplace_back(v);
    }
    make_block_entry(def);
}

void instruction_block::reserve_byte(size_t count)
{
    make_block_entry(data_definition_t {
        .size = op_sizes::byte,
        .values = {count},
        .type = data_definition_type_t::uninitialized,
    });
}

void instruction_block::reserve_word(size_t count)
{
    make_block_entry(data_definition_t {
        .size = op_sizes::word,
        .values = {count},
        .type = data_definition_type_t::uninitialized,
    });
}

void instruction_block::reserve_dword(size_t count)
{
    make_block_entry(data_definition_t {
        .size = op_sizes::dword,
        .values = {count},
        .type = data_definition_type_t::uninitialized,
    });
}

void instruction_block::reserve_qword(size_t count)
{
    make_block_entry(data_definition_t {
        .size = op_sizes::qword,
        .values = {count},
        .type = data_definition_type_t::uninitialized,
    });
}

void instruction_block::string(const std::string &value)
{
    dwords({static_cast<uint32_t>(value.length())});
    std::vector<uint8_t> str_bytes {};
    for (const auto& c : value) {
        str_bytes.emplace_back(static_cast<uint8_t>(c));
    }
    str_bytes.emplace_back(0);
    bytes(str_bytes);
}

void instruction_block::blank_line()
{
    entries_.emplace_back(block_entry_t());
}

void instruction_block::label(gfx::label* value)
{
    make_block_entry(label_t {
        .instance = value
    });
}

void instruction_block::make_block_entry(const instruction_t &inst)
{
    entries_.emplace_back(block_entry_t(inst));
}

void instruction_block::make_block_entry(const label_t& label)
{
    entries_.emplace_back(label);
}

void instruction_block::make_block_entry(const section_t &section)
{
    entries_.emplace_back(block_entry_t(section));
}

void instruction_block::make_block_entry(const comment_t& comment)
{
    entries_.emplace_back(comment);
}

void instruction_block::comment(const std::string& value, uint8_t indent)
{
    make_block_entry(comment_t {
        .indent = indent,
        .value = value,
    });
}

void instruction_block::make_block_entry(const align_t &align)
{
    entries_.emplace_back(align);
}

void instruction_block::make_block_entry(const data_definition_t &data)
{
    entries_.emplace_back(block_entry_t(data));
}

void instruction_block::clear_entries()
{
    entries_.clear();
}

void instruction_block::align(uint8_t size)
{
    make_block_entry(align_t {
        .size = size
    });
}

listing_source_file_t *instruction_block::source_file()
{
    return source_file_;
}

void instruction_block::source_file(listing_source_file_t *value)
{
    source_file_ = value;
}


std::vector<block_entry_t> &instruction_block::entries()
{
    return entries_;
}

void instruction_block::move_label_to_reg_with_offset(const register_t& dest_reg,
    const label_ref_t *label_ref, uint64_t offset)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = op_sizes::qword;
    move_op.operands_count = 3;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg.number;
    move_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    move_op.operands[1].value.u = label_ref->id;
    move_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant;
    move_op.operands[2].value.u = offset;
    make_block_entry(move_op);
}

// div variations
void instruction_block::div_reg_by_reg(const register_t& dest_reg, const register_t& dividend_reg, const register_t& divisor_reg)
{
    make_div_instruction(dest_reg.size, dest_reg, dividend_reg, divisor_reg);
}

// mod variations
void instruction_block::mod_reg_by_reg(const register_t& dest_reg, const register_t& dividend_reg, const register_t& divisor_reg)
{
    make_mod_instruction(dest_reg.size, dest_reg, dividend_reg, divisor_reg);
}

// swap variations
void instruction_block::swap_reg_with_reg(const register_t& dest_reg, const register_t& src_reg)
{
    make_swap_instruction(dest_reg.size, dest_reg, src_reg);
}

// test mask for zero and branch
void instruction_block::test_mask_branch_if_zero(const register_t& value_reg, const register_t& mask_reg,
    const register_t& address_reg)
{

}

// test mask for non-zero and branch
void instruction_block::test_mask_branch_if_not_zero(const register_t& value_reg,
    const register_t& mask_reg, const register_t& address_reg)
{

}

///
void instruction_block::push(const register_t& reg)
{
    make_push_instruction(reg);
}

void instruction_block::pop(const register_t& reg)
{
    make_pop_instruction(reg);
}

/// load variations
void instruction_block::load_to_reg(const register_t& dest_reg, const register_t& address_reg, int64_t offset)
{
    make_load_instruction(dest_reg.size, dest_reg, address_reg, offset);
}

/// store
void instruction_block::store_from_reg(const register_t& address_reg, const register_t& src_reg, int64_t offset)
{
    make_store_instruction(src_reg.size, address_reg, src_reg, offset);
}

// neg variations
void instruction_block::neg(const register_t& dest_reg, const register_t& src_reg)
{
    make_neg_instruction(dest_reg.size, dest_reg, src_reg);
}

// or variations
void instruction_block::or_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_or_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

// xor variations
void instruction_block::xor_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_xor_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

// and variations
void instruction_block::and_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_and_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

// shl variations
void instruction_block::shl_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_shl_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

// shr variations
void instruction_block::shr_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_shr_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

// rol variations
void instruction_block::rol_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_rol_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

// ror variations
void instruction_block::ror_reg_by_reg(const register_t& dest_reg, const register_t& lhs_reg, const register_t& rhs_reg)
{
    make_ror_instruction(dest_reg.size, dest_reg, lhs_reg, rhs_reg);
}

/// move
void instruction_block::move_constant_to_reg(const register_t& dest_reg, uint64_t immediate)
{
    make_move_instruction(dest_reg.size, dest_reg, immediate);
}

void instruction_block::move_constant_to_reg(const register_t& dest_reg, double immediate)
{
    make_move_instruction(dest_reg.size, dest_reg, immediate);
}



// inc variations
void instruction_block::inc(const register_t& reg)
{
    make_inc_instruction(reg.size, reg);
}

// not variations
void instruction_block::not_op(const register_t& dest_reg, const register_t& src_reg)
{
    make_not_instruction(dest_reg.size, dest_reg, src_reg);
}

// dec variations
void instruction_block::dec(const register_t& reg)
{
    make_dec_instruction(reg.size, reg);
}

void instruction_block::mul_reg_by_reg(const register_t& dest_reg, const register_t& multiplicand_reg, const register_t& multiplier_reg)
{
    make_mul_instruction(dest_reg.size, dest_reg, multiplicand_reg, multiplier_reg);
}

// add variations
void instruction_block::add_reg_by_reg(const register_t& dest_reg, const register_t& augend_reg, const register_t& addened_reg)
{
    make_add_instruction(dest_reg.size, dest_reg, augend_reg, addened_reg);
}

// sub variations
void instruction_block::sub_reg_by_reg(const register_t& dest_reg, const register_t& augend_reg, const register_t& addened_reg)
{
    make_sub_instruction(dest_reg.size, dest_reg, augend_reg, addened_reg);
}

void instruction_block::moves_reg_to_reg(const register_t &dest_reg, const register_t &src_reg)
{
    make_move_instruction(dest_reg.size, op_codes::moves, dest_reg, src_reg);
}

void instruction_block::movez_reg_to_reg(const register_t &dest_reg, const register_t &src_reg)
{
    make_move_instruction(dest_reg.size, op_codes::movez, dest_reg, src_reg);
}

void instruction_block::convert(const register_t &dest_reg, const register_t &src_reg)
{
    instruction_t convert_op;
    convert_op.op = op_codes::convert;
    convert_op.size = dest_reg.size;
    convert_op.operands_count = 2;
    convert_op.operands[0].value.r = dest_reg.number;
    convert_op.operands[0].type = operand_encoding_t::flags::reg;
    if (dest_reg.type == register_type_t::integer) {
        convert_op.operands[0].type |= operand_encoding_t::flags::integer;
    }

    convert_op.operands[1].value.r = src_reg.number;
    convert_op.operands[1].type = operand_encoding_t::flags::reg;
    if (src_reg.type == register_type_t::integer) {
        convert_op.operands[1].type |= operand_encoding_t::flags::integer;
    }
    make_block_entry(convert_op);
}

void instruction_block::add_entry(const block_entry_t &entry)
{
    entries_.emplace_back(entry);
}

bool instruction_block::should_emit() const
{
    return shoudle_emitted_;
}

void instruction_block::should_emit(bool value)
{
    shoudle_emitted_ = value;
}

}