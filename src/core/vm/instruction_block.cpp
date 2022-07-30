//
// Created by 12132 on 2022/7/17.
//

#include "instruction_block.h"
#include "fmt/color.h"
namespace gfx {

instruction_block::instruction_block(instruction_block* parent, instruction_block_type_t type)
    : stack_frame_(parent), parent_(parent), type_(type)
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
    auto label_ref = make_unresolved_label_ref(proc_name);
    instruction_t jsr_op;
    jsr_op.op = op_codes::jsr;
    jsr_op.size = op_sizes::qword;
    jsr_op.operands_count = 1;
    jsr_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant
            | operand_encoding_t::flags::unresolved;
    jsr_op.operands[0].value.u64 = label_ref->id;
    instructions_.push_back(jsr_op);

// XXX: this is a PC-relative encoding
//        instruction_t jsr_op;
//        jsr_op.op = op_codes::jsr;
//        jsr_op.size = size;
//        jsr_op.operands_count = 2;
//        jsr_op.operands[0].type =
//            operand_encoding_t::flags::integer
//            | operand_encoding_t::flags::reg;
//        jsr_op.operands[0].value.r8 = i_registers_t::pc;
//        jsr_op.operands[1].type = offset_type | operand_encoding_t::flags::integer;
//        jsr_op.operands[1].value.u64 = offset;
//        _instructions.push_back(jsr_op);
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

bool instruction_block::allocate_reg(i_registers_t& reg)
{
    return i_register_allocator_.allocate(reg);
}

bool instruction_block::allocate_reg(f_registers_t& reg)
{
    return f_register_allocator_.allocate(reg);
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
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t jmp_op;
    jmp_op.op = op_codes::jmp;
    jmp_op.size = op_sizes::qword;
    jmp_op.operands_count = 1;
    jmp_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant
            | operand_encoding_t::flags::unresolved;
    jmp_op.operands[0].value.u64 = label_ref->id;
    instructions_.push_back(jmp_op);
}

void instruction_block::disassemble()
{
    disassemble(this);
}

void instruction_block::free_reg(i_registers_t reg)
{
    i_register_allocator_.free(reg);
}

void instruction_block::free_reg(f_registers_t reg)
{
    f_register_allocator_.free(reg);
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

void instruction_block::make_store_instruction(op_sizes size, i_registers_t address_reg,
                                               i_registers_t src_reg,int64_t offset)
{
    instruction_t store_op;
    store_op.op = op_codes::store;
    store_op.size = size;
    store_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    store_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    store_op.operands[0].value.r8 = address_reg;
    store_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    store_op.operands[1].value.r8 = src_reg;

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
    move_op.operands[0].type =  operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r8 = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer |
        operand_encoding_t::flags::constant;
    move_op.operands[1].value.u64 = value;
    instructions_.push_back(move_op);
}

void instruction_block::make_move_instruction(op_sizes size, f_registers_t dest_reg, double value)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::reg;
    move_op.operands[0].value.r8 = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::constant;
    move_op.operands[1].value.d64 = value;
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
    move_op.operands[0].value.r8 = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    move_op.operands[1].value.r8 = src_reg;
    instructions_.push_back(move_op);
}

void instruction_block::disassemble(instruction_block *block)
{
    for (const auto& it : block->labels_) {
        fmt::print("{}:\n", it.first);
    }
    size_t index = 0;
    for (const auto& inst : block->instructions_) {
        auto it = block->comments_.find(index);
        if (it != block->comments_.end()) {
            for (const auto& line : it->second.lines) {
                fmt::print("\t; {}\n", line);
            }
        }
        auto stream = inst.disassemble([&](uint64_t id) -> std::string {
          auto label_ref = block->find_unresolved_label_up(static_cast<id_t>(id));
          return label_ref != nullptr ? label_ref->name : fmt::format("unresolved_ref_id({})", id);
        });
        fmt::print("\t{}\n", stream);
        index++;
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
    add_op.operands[2].type = operand_encoding_t::flags::integer
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

void instruction_block::move_ireg_to_ireg(i_registers_t dest_reg, i_registers_t src_reg)
{
    make_move_instruction(op_sizes::qword, dest_reg, src_reg);
}

void instruction_block::make_cmp_instruction(op_sizes size, i_registers_t lhs_reg, i_registers_t rhs_reg)
{
    instruction_t cmp_op;
    cmp_op.op = op_codes::cmp;
    cmp_op.size = size;
    cmp_op.operands_count = 2;
    cmp_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    cmp_op.operands[0].value.r8 = lhs_reg;
    cmp_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    cmp_op.operands[1].value.r8 = rhs_reg;
    instructions_.push_back(cmp_op);
}

void instruction_block::make_not_instruction(op_sizes size, i_registers_t dest_reg, i_registers_t src_reg)
{
    instruction_t not_op;
    not_op.op = op_codes::not_op;
    not_op.size = size;
    not_op.operands_count = 2;
    not_op.operands[0].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    not_op.operands[0].value.r8 = dest_reg;
    not_op.operands[1].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    not_op.operands[1].value.r8 = src_reg;
    instructions_.push_back(not_op);
}

target_register_t instruction_block::pop_target_register()
{
    if (target_registers_.empty()) {
        return target_register_t {};
    }
    auto reg = target_registers_.top();
    target_registers_.pop();
    return reg;
}

void instruction_block::beq(const std::string &label_name)
{
    auto label_ref = make_unresolved_label_ref(label_name);
    instruction_t branch_op;
    branch_op.op = op_codes::beq;
    branch_op.size = op_sizes::qword;
    branch_op.operands_count = 1;
    branch_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    branch_op.operands[0].value.u64 = label_ref->id;
    instructions_.push_back(branch_op);
}

void instruction_block::comment(const std::string &value)
{
    auto index = instructions_.size();
    auto it = comments_.find(index);
    if (it == comments_.end()) {
        instruction_comments_t comments {};
        comments.lines.push_back(value);
        comments_.insert(std::make_pair(index, comments));
    } else {
        auto& comments = it->second;
        comments.lines.push_back(value);
    }
}

target_register_t *instruction_block::current_target_register()
{
    if (target_registers_.empty()) {
        return nullptr;
    }
    return &target_registers_.top();
}

void instruction_block::push_target_register(i_registers_t reg)
{
    target_register_t target {
        .type = target_register_type_t::integer,
        .reg = {
            .i = reg
        }
    };
    target_registers_.push(target);
}

void instruction_block::push_target_register(f_registers_t reg)
{
    target_register_t target {
        .type = target_register_type_t::floating_point,
        .reg = {
            .f = reg
        }
    };
    target_registers_.push(target);
}

void instruction_block::cmp_u64(i_registers_t lhs_reg, i_registers_t rhs_reg)
{
    make_cmp_instruction(op_sizes::qword, lhs_reg, rhs_reg);
}

void instruction_block::bne(const std::string &label_name)
{
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t branch_op;
    branch_op.op = op_codes::bne;
    branch_op.size = op_sizes::qword;
    branch_op.operands_count = 1;
    branch_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::constant
            | operand_encoding_t::flags::unresolved;
    branch_op.operands[0].value.u64 = label_ref->id;
    instructions_.push_back(branch_op);
}

void instruction_block::make_shl_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t amount_reg)
{
    instruction_t shift_op;
    shift_op.op = op_codes::shl;
    shift_op.size = size;
    shift_op.operands_count = 3;
    shift_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    shift_op.operands[0].value.r8 = dest_reg;
    shift_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    shift_op.operands[1].value.r8 = value_reg;
    shift_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    shift_op.operands[2].value.r8 = amount_reg;
    instructions_.push_back(shift_op);
}
void instruction_block::make_rol_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t amount_reg)
{
    instruction_t rotate_op;
    rotate_op.op = op_codes::rol;
    rotate_op.size = size;
    rotate_op.operands_count = 3;
    rotate_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    rotate_op.operands[0].value.r8 = dest_reg;
    rotate_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    rotate_op.operands[1].value.r8 = value_reg;
    rotate_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    rotate_op.operands[2].value.r8 = amount_reg;
    instructions_.push_back(rotate_op);
}

void instruction_block::make_shr_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t amount_reg)
{
    instruction_t shift_op;
    shift_op.op = op_codes::shr;
    shift_op.size = size;
    shift_op.operands_count = 3;
    shift_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    shift_op.operands[0].value.r8 = dest_reg;
    shift_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    shift_op.operands[1].value.r8 = value_reg;
    shift_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    shift_op.operands[2].value.r8 = amount_reg;
    instructions_.push_back(shift_op);
}

void instruction_block::make_ror_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t amount_reg)
{
    instruction_t rotate_op;
    rotate_op.op = op_codes::ror;
    rotate_op.size = size;
    rotate_op.operands_count = 3;
    rotate_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    rotate_op.operands[0].value.r8 = dest_reg;
    rotate_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    rotate_op.operands[1].value.r8 = value_reg;
    rotate_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    rotate_op.operands[2].value.r8 = amount_reg;
    instructions_.push_back(rotate_op);
}
void instruction_block::make_and_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t mask_reg)
{
    instruction_t and_op;
    and_op.op = op_codes::and_op;
    and_op.size = size;
    and_op.operands_count = 3;
    and_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    and_op.operands[0].value.r8 = dest_reg;
    and_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    and_op.operands[1].value.r8 = value_reg;
    and_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    and_op.operands[2].value.r8 = mask_reg;
    instructions_.push_back(and_op);
}

void instruction_block::make_xor_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t mask_reg)
{
    instruction_t xor_op;
    xor_op.op = op_codes::xor_op;
    xor_op.size = size;
    xor_op.operands_count = 3;
    xor_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    xor_op.operands[0].value.r8 = dest_reg;
    xor_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    xor_op.operands[1].value.r8 = value_reg;
    xor_op.operands[2].type =
        operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    xor_op.operands[2].value.r8 = mask_reg;
    instructions_.push_back(xor_op);
}

void instruction_block::make_or_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t value_reg, i_registers_t mask_reg)
{
    instruction_t or_op;
    or_op.op = op_codes::or_op;
    or_op.size = size;
    or_op.operands_count = 3;
    or_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    or_op.operands[0].value.r8 = dest_reg;
    or_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    or_op.operands[1].value.r8 = value_reg;
    or_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    or_op.operands[2].value.r8 = mask_reg;
    instructions_.push_back(or_op);
}
void instruction_block::make_mod_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t dividend_reg, i_registers_t divisor_reg)
{
    instruction_t mod_op;
    mod_op.op = op_codes::mod;
    mod_op.size = size;
    mod_op.operands_count = 3;
    mod_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    mod_op.operands[0].value.r8 = dest_reg;
    mod_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    mod_op.operands[1].value.r8 = dividend_reg;
    mod_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    mod_op.operands[2].value.r8 = divisor_reg;
    instructions_.push_back(mod_op);
}

void instruction_block::make_div_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t dividend_reg, i_registers_t divisor_reg)
{
    instruction_t div_op;
    div_op.op = op_codes::div;
    div_op.size = size;
    div_op.operands_count = 3;
    div_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    div_op.operands[0].value.r8 = dest_reg;
    div_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    div_op.operands[1].value.r8 = dividend_reg;
    div_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    div_op.operands[2].value.r8 = divisor_reg;
    instructions_.push_back(div_op);
}

void instruction_block::make_mul_instruction(op_sizes size, i_registers_t dest_reg,
    i_registers_t multiplicand_reg, i_registers_t multiplier_reg)
{
    instruction_t mul_op;
    mul_op.op = op_codes::mul;
    mul_op.size = size;
    mul_op.operands_count = 3;
    mul_op.operands[0].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    mul_op.operands[0].value.r8 = dest_reg;
    mul_op.operands[1].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    mul_op.operands[1].value.r8 = multiplicand_reg;
    mul_op.operands[2].type = operand_encoding_t::flags::integer
            | operand_encoding_t::flags::reg;
    mul_op.operands[2].value.r8 = multiplier_reg;
    instructions_.push_back(mul_op);
}
void instruction_block::setz(i_registers_t dest_reg)
{
    instruction_t setz_op;
    setz_op.op = op_codes::setz;
    setz_op.size = op_sizes::qword;
    setz_op.operands_count = 1;
    setz_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    setz_op.operands[0].value.r8 = dest_reg;
    instructions_.push_back(setz_op);
}

void instruction_block::setnz(i_registers_t dest_reg)
{
    instruction_t setnz_op;
    setnz_op.op = op_codes::setnz;
    setnz_op.size = op_sizes::qword;
    setnz_op.operands_count = 1;
    setnz_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    setnz_op.operands[0].value.r8 = dest_reg;
    instructions_.push_back(setnz_op);
}
void instruction_block::sub_ireg_by_immediate(i_registers_t dest_reg, i_registers_t minuend_reg,
                                              uint64_t subtrahend_immediate)
{

}
stack_frame_t *instruction_block::stack_frame()
{
    return &stack_frame_;
}

void instruction_block::make_sub_instruction_immediate(op_sizes size, i_registers_t dest_reg,
    i_registers_t minuend_reg, uint64_t subtrahend_immediate)
{

}

}