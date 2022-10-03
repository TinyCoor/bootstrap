//
// Created by 12132 on 2022/7/17.
//

#include <sstream>
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
    clear_entries();

    for (const auto& it : labels_) {
        delete it.second;
    }
    labels_.clear();
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
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    jsr_op.operands[0].value.u = label_ref->id;
    make_block_entry(jsr_op);

// XXX: this is a PC-relative encoding
//        instruction_t jsr_op;
//        jsr_op.op = op_codes::jsr;
//        jsr_op.size = size;
//        jsr_op.operands_count = 2;
//        jsr_op.operands[0].type =
//            operand_encoding_t::flags::integer
//            | operand_encoding_t::flags::reg;
//        jsr_op.operands[0].value.r8 = registers_t::pc;
//        jsr_op.operands[1].type = offset_type | operand_encoding_t::flags::integer;
//        jsr_op.operands[1].value.u64 = offset;
//        _instructions.push_back(jsr_op);
}

label* instruction_block::make_label(const std::string& name)
{
    auto it = labels_.insert(std::make_pair(name, new class label(name)));
    return it.first->second;
}

void instruction_block::clear_blocks()
{
    blocks_.clear();
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

void instruction_block::jump_indirect(registers_t reg)
{
    instruction_t jmp_op;
    jmp_op.op = op_codes::jmp;
    jmp_op.size = op_sizes::qword;
    jmp_op.operands_count = 1;
    jmp_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    jmp_op.operands[0].value.r = reg;
    make_block_entry(jmp_op);
}

void instruction_block::jump_direct(const std::string &label_name)
{
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t jmp_op;
    jmp_op.op = op_codes::jmp;
    jmp_op.size = op_sizes::qword;
    jmp_op.operands_count = 1;
    jmp_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    jmp_op.operands[0].value.u = label_ref->id;
    make_block_entry(jmp_op);
}

void instruction_block::disassemble()
{
    disassemble(this);
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

void instruction_block::make_inc_instruction(op_sizes size, registers_t reg)
{
    instruction_t inc_op;
    inc_op.op = op_codes::inc;
    inc_op.size = size;
    inc_op.operands_count = 1;
    inc_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    inc_op.operands[0].value.r = reg;
    make_block_entry(inc_op);
}

void instruction_block::make_dec_instruction(op_sizes size, registers_t reg)
{
    instruction_t dec_op;
    dec_op.op = op_codes::dec;
    dec_op.operands_count = 1;
    dec_op.size = size;
    dec_op.operands[0].value.r = reg;
    dec_op.operands[0].type =operand_encoding_t::integer | operand_encoding_t::reg;
    make_block_entry(dec_op);
}

void instruction_block::make_load_instruction(op_sizes size, registers_t dest_reg,
    registers_t address_reg, int64_t offset)
{
    instruction_t load_op;
    load_op.op = op_codes::load;
    load_op.size = size;
    load_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    load_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    load_op.operands[0].value.r = dest_reg;
    load_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    load_op.operands[1].value.r = address_reg;
    if (load_op.operands_count == 3) {
        load_op.operands[2].type = operand_encoding_t::flags::integer | operand_encoding_t::flags::constant;
        if (offset < 0) {
            load_op.operands[2].type |= operand_encoding_t::flags::negative;
        }
        load_op.operands[2].value.u = static_cast<uint64_t>(offset);
    }
    make_block_entry(load_op);
}

void instruction_block::make_store_instruction(op_sizes size, registers_t address_reg,
    registers_t src_reg, int64_t offset)
{
    instruction_t store_op;
    store_op.op = op_codes::store;
    store_op.size = size;
    store_op.operands_count = static_cast<uint8_t>(offset != 0 ? 3 : 2);
    store_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    store_op.operands[0].value.r = address_reg;
    store_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    store_op.operands[1].value.r = src_reg;

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

void instruction_block::make_swap_instruction(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    instruction_t swap_op;
    swap_op.op = op_codes::swap;
    swap_op.size = size;
    swap_op.operands_count = 2;
    swap_op.operands[0].type = operand_encoding_t::flags::integer
       | operand_encoding_t::flags::reg;
    swap_op.operands[0].value.r = dest_reg;
    swap_op.operands[1].type = operand_encoding_t::flags::integer
       | operand_encoding_t::flags::reg;
    swap_op.operands[1].value.r = src_reg;
    make_block_entry(swap_op);
}

void instruction_block::make_pop_instruction(op_sizes size, registers_t dest_reg)
{
    instruction_t pop_op;
    pop_op.op = op_codes::pop;
    pop_op.size = size;
    pop_op.operands_count = 1;
    pop_op.operands[0].type = operand_encoding_t::flags::integer
       | operand_encoding_t::flags::reg;
    pop_op.operands[0].value.r = dest_reg;
    make_block_entry(pop_op);
}

void instruction_block::make_move_instruction(op_sizes size, registers_t dest_reg, uint64_t value)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer |
        operand_encoding_t::flags::constant;
    move_op.operands[1].value.u = value;
    make_block_entry(move_op);
}


void instruction_block::make_move_instruction(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = size;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[1].value.r = src_reg;
    make_block_entry(move_op);
}

void instruction_block::disassemble(instruction_block *block)
{
    auto source_file = block->source_file();
    if (source_file == nullptr) {
        return;
    }

    size_t index = 0;
    for (auto& entry : block->entries_) {
        source_file->add_blank_lines(entry.address(), entry.blank_lines());
        for (const auto& comment : entry.comments()) {
            source_file->add_source_line(entry.address(), fmt::format("; {}", comment));
        }

        if (entry.type() == block_entry_type_t::align) {
            auto align = entry.data<align_t>();
            source_file->add_source_line(entry.address(), fmt::format(".align {}", align->size));
        } else if (entry.type() == block_entry_type_t::section) {
            auto section = entry.data<section_t>();
            source_file->add_source_line(entry.address(), fmt::format(".section '{}'", section_name(*section)));
        }

        for (auto label : entry.labels()) {
            source_file->add_source_line(entry.address(), fmt::format("{}:", label->name()));
        }
        switch (entry.type()) {
            case block_entry_type_t::memo:
            case block_entry_type_t::align:
            case block_entry_type_t::section: {
               break;
            }
            case block_entry_type_t::instruction: {
                auto inst = entry.data<instruction_t>();
                auto stream = inst->disassemble([&](uint64_t id) -> std::string {
                  auto label_ref = block->find_unresolved_label_up(static_cast<id_t>(id));
                  if (label_ref !=nullptr) {
                      if (label_ref->resolved !=nullptr) {
                          return fmt::format("{} (${:08x})", label_ref->name, label_ref->resolved->address());
                      } else {
                          return label_ref->name;
                      }
                  }
                  return fmt::format("unresolved_ref_id({})", id);
                });
                source_file->add_source_line(entry.address(), fmt::format("\t{}", stream));
                break;
            }
            case block_entry_type_t::data_definition: {
                auto definition = entry.data<data_definition_t>();
                std::stringstream directive;
                std::string format_spec;
                switch (definition->type) {
                    case data_definition_type_t::initialized : {
                        switch (definition->size) {
                            case op_sizes::byte:
                                directive << ".db";
                                format_spec = "${:02X}";
                                break;
                            case op_sizes::word:
                                directive << ".dw";
                                format_spec = "${:04X}";
                                break;
                            case op_sizes::dword:
                                directive << ".dd";
                                format_spec = "${:08X}";
                                break;
                            case op_sizes::qword:
                                directive << ".dq";
                                format_spec = "${:016X}";
                                break;
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case data_definition_type_t::uninitialized: {
                        format_spec = "${:04X}";
                        switch (definition->size) {
                            case op_sizes::byte:
                                directive << ".db";
                                break;
                            case op_sizes::word:
                                directive << ".dw";
                                break;
                            case op_sizes::dword:
                                directive << ".dd";
                                break;
                            case op_sizes::qword:
                                directive << ".dq";
                                break;
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                }

                auto item_index = 0;
                auto item_count = definition->values.size();
                std::string items;
                while (item_count > 0) {
                    if (!items.empty()) {
                        items += ", ";
                    }
                    items += fmt::format(fmt::runtime(format_spec), definition->values[item_index++]);
                    if ((item_index % 8) == 0) {
                        source_file->add_source_line(entry.address(), fmt::format("\t{:<10}{}", directive.str(), items));
                        items.clear();
                    }
                    --item_count;
                }
                if (!items.empty()) {
                    source_file->add_source_line(
                        entry.address(),
                        fmt::format("\t{:<10}{}", directive.str(), items));
                }
                break;
            }
        }
        index++;
    }

    for (auto child_block : block->blocks_) {
        disassemble(child_block);
    }
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

void instruction_block::make_push_instruction(op_sizes size, registers_t reg)
{
    instruction_t push_op;
    push_op.op = op_codes::push;
    push_op.size = size;
    push_op.operands_count = 1;
    push_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    push_op.operands[0].value.r = reg;
    make_block_entry(push_op);
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

void instruction_block::move_label_to_ireg(registers_t dest_reg, const std::string &label_name)
{
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = op_sizes::qword;
    move_op.operands_count = 2;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    move_op.operands[1].value.u = label_ref->id;
    make_block_entry(move_op);
}

instruction_block *instruction_block::parent()
{
    return parent_;
}

void instruction_block::make_neg_instruction(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    instruction_t neg_op;
    neg_op.op = op_codes::neg;
    neg_op.size = size;
    neg_op.operands_count = 2;
    neg_op.operands[0].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    neg_op.operands[0].value.r = dest_reg;
    neg_op.operands[1].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    neg_op.operands[1].value.r = src_reg;
    make_block_entry(neg_op);
}

void instruction_block::make_add_instruction(op_sizes size,
    registers_t dest_reg, registers_t augend_reg, registers_t addend_reg)
{
    instruction_t add_op;
    add_op.op = op_codes::add;
    add_op.size = size;
    add_op.operands_count = 3;
    add_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    add_op.operands[0].value.r = dest_reg;
    add_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    add_op.operands[1].value.r = augend_reg;
    add_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    add_op.operands[2].value.r = addend_reg;
    make_block_entry(add_op);
}

void instruction_block::make_sub_instruction(op_sizes size, registers_t dest_reg,
    registers_t minuend_reg, registers_t subtrahend_reg)
{
    instruction_t sub_op;
    sub_op.op = op_codes::sub;
    sub_op.size = size;
    sub_op.operands_count = 3;
    sub_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[0].value.r = dest_reg;
    sub_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[1].value.r = minuend_reg;
    sub_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[2].value.r = subtrahend_reg;
    make_block_entry(sub_op);
}

void instruction_block::move_ireg_to_ireg(registers_t dest_reg, registers_t src_reg)
{
    make_move_instruction(op_sizes::qword, dest_reg, src_reg);
}

void instruction_block::make_cmp_instruction(op_sizes size, registers_t lhs_reg, registers_t rhs_reg)
{
    instruction_t cmp_op;
    cmp_op.op = op_codes::cmp;
    cmp_op.size = size;
    cmp_op.operands_count = 2;
    cmp_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    cmp_op.operands[0].value.r = lhs_reg;
    cmp_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    cmp_op.operands[1].value.r = rhs_reg;
    make_block_entry(cmp_op);
}

void instruction_block::make_not_instruction(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    instruction_t not_op;
    not_op.op = op_codes::not_op;
    not_op.size = size;
    not_op.operands_count = 2;
    not_op.operands[0].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    not_op.operands[0].value.r = dest_reg;
    not_op.operands[1].type = operand_encoding_t::flags::reg | operand_encoding_t::flags::integer;
    not_op.operands[1].value.r = src_reg;
    make_block_entry(not_op);
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
    branch_op.operands[0].value.u = label_ref->id;
    make_block_entry(branch_op);
}

void instruction_block::cmp(op_sizes sizes, registers_t lhs_reg, registers_t rhs_reg)
{
    make_cmp_instruction(sizes, lhs_reg, rhs_reg);
}

void instruction_block::bne(const std::string &label_name)
{
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t branch_op;
    branch_op.op = op_codes::bne;
    branch_op.size = op_sizes::qword;
    branch_op.operands_count = 1;
    branch_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    branch_op.operands[0].value.u = label_ref->id;
    make_block_entry(branch_op);
}

void instruction_block::make_shl_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t amount_reg)
{
    instruction_t shift_op;
    shift_op.op = op_codes::shl;
    shift_op.size = size;
    shift_op.operands_count = 3;
    shift_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[0].value.r = dest_reg;
    shift_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[1].value.r = value_reg;
    shift_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[2].value.r = amount_reg;
    make_block_entry(shift_op);
}

void instruction_block::make_rol_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t amount_reg)
{
    instruction_t rotate_op;
    rotate_op.op = op_codes::rol;
    rotate_op.size = size;
    rotate_op.operands_count = 3;
    rotate_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[0].value.r = dest_reg;
    rotate_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[1].value.r = value_reg;
    rotate_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[2].value.r = amount_reg;
    make_block_entry(rotate_op);
}

void instruction_block::make_shr_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t amount_reg)
{
    instruction_t shift_op;
    shift_op.op = op_codes::shr;
    shift_op.size = size;
    shift_op.operands_count = 3;
    shift_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[0].value.r = dest_reg;
    shift_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[1].value.r = value_reg;
    shift_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    shift_op.operands[2].value.r = amount_reg;
    make_block_entry(shift_op);
}

void instruction_block::make_ror_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t amount_reg)
{
    instruction_t rotate_op;
    rotate_op.op = op_codes::ror;
    rotate_op.size = size;
    rotate_op.operands_count = 3;
    rotate_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[0].value.r = dest_reg;
    rotate_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[1].value.r = value_reg;
    rotate_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    rotate_op.operands[2].value.r = amount_reg;
    make_block_entry(rotate_op);
}

void instruction_block::make_and_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t mask_reg)
{
    instruction_t and_op;
    and_op.op = op_codes::and_op;
    and_op.size = size;
    and_op.operands_count = 3;
    and_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    and_op.operands[0].value.r = dest_reg;
    and_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    and_op.operands[1].value.r = value_reg;
    and_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    and_op.operands[2].value.r = mask_reg;
    make_block_entry(and_op);
}

void instruction_block::make_xor_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t mask_reg)
{
    instruction_t xor_op;
    xor_op.op = op_codes::xor_op;
    xor_op.size = size;
    xor_op.operands_count = 3;
    xor_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    xor_op.operands[0].value.r = dest_reg;
    xor_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    xor_op.operands[1].value.r = value_reg;
    xor_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    xor_op.operands[2].value.r = mask_reg;
    make_block_entry(xor_op);
}

void instruction_block::make_or_instruction(op_sizes size, registers_t dest_reg,
    registers_t value_reg, registers_t mask_reg)
{
    instruction_t or_op;
    or_op.op = op_codes::or_op;
    or_op.size = size;
    or_op.operands_count = 3;
    or_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    or_op.operands[0].value.r = dest_reg;
    or_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    or_op.operands[1].value.r = value_reg;
    or_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    or_op.operands[2].value.r = mask_reg;
    make_block_entry(or_op);
}

void instruction_block::make_mod_instruction(op_sizes size, registers_t dest_reg,
    registers_t dividend_reg, registers_t divisor_reg)
{
    instruction_t mod_op;
    mod_op.op = op_codes::mod;
    mod_op.size = size;
    mod_op.operands_count = 3;
    mod_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mod_op.operands[0].value.r = dest_reg;
    mod_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mod_op.operands[1].value.r = dividend_reg;
    mod_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mod_op.operands[2].value.r = divisor_reg;
    make_block_entry(mod_op);
}

void instruction_block::make_div_instruction(op_sizes size, registers_t dest_reg,
    registers_t dividend_reg, registers_t divisor_reg)
{
    instruction_t div_op;
    div_op.op = op_codes::div;
    div_op.size = size;
    div_op.operands_count = 3;
    div_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    div_op.operands[0].value.r = dest_reg;
    div_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    div_op.operands[1].value.r = dividend_reg;
    div_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    div_op.operands[2].value.r = divisor_reg;
    make_block_entry(div_op);
}

void instruction_block::make_mul_instruction(op_sizes size, registers_t dest_reg,
    registers_t multiplicand_reg, registers_t multiplier_reg)
{
    instruction_t mul_op;
    mul_op.op = op_codes::mul;
    mul_op.size = size;
    mul_op.operands_count = 3;
    mul_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mul_op.operands[0].value.r = dest_reg;
    mul_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mul_op.operands[1].value.r = multiplicand_reg;
    mul_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    mul_op.operands[2].value.r = multiplier_reg;
    make_block_entry(mul_op);
}

void instruction_block::setz(registers_t dest_reg)
{
    instruction_t setz_op;
    setz_op.op = op_codes::setz;
    setz_op.size = op_sizes::qword;
    setz_op.operands_count = 1;
    setz_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    setz_op.operands[0].value.r = dest_reg;
    make_block_entry(setz_op);
}

void instruction_block::setnz(registers_t dest_reg)
{
    instruction_t setnz_op;
    setnz_op.op = op_codes::setnz;
    setnz_op.size = op_sizes::qword;
    setnz_op.operands_count = 1;
    setnz_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    setnz_op.operands[0].value.r = dest_reg;
    make_block_entry(setnz_op);
}

void instruction_block::sub_ireg_by_immediate(op_sizes size, registers_t dest_reg, registers_t minuend_reg,
    uint64_t subtrahend_immediate)
{
    make_sub_instruction_immediate(size, dest_reg, minuend_reg, subtrahend_immediate);
}

stack_frame_t *instruction_block::stack_frame()
{
    return &stack_frame_;
}

void instruction_block::make_sub_instruction_immediate(op_sizes size, registers_t dest_reg,
    registers_t minuend_reg, uint64_t subtrahend_immediate)
{
    instruction_t sub_op;
    sub_op.op = op_codes::sub;
    sub_op.size = size;
    sub_op.operands_count = 3;
    sub_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[0].value.r = dest_reg;
    sub_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    sub_op.operands[1].value.r = minuend_reg;
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

void instruction_block::make_block_entry(const instruction_t &inst)
{
    entries_.emplace_back(block_entry_t(inst));
}

void instruction_block::make_block_entry(const section_t &section)
{
    entries_.emplace_back(block_entry_t(section));
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

block_entry_t *instruction_block::current_entry()
{
    if (entries_.empty()) {
        return nullptr;
    }
    return &entries_.back();
}

void instruction_block::memo()
{
    entries_.emplace_back(block_entry_t());
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

std::vector<label_ref_t *> instruction_block::label_references()
{
    std::vector<label_ref_t*> refs {};
    for (auto& kvp : unresolved_labels_) {
        refs.push_back(&kvp.second);
    }
    return refs;
}
std::vector<block_entry_t> &instruction_block::entries()
{
    return entries_;
}

label *instruction_block::find_label(const std::string &name)
{
    return find_in_blocks<label>([&](instruction_block* block) -> label* {
      const auto it = block->labels_.find(name);
      if (it == block->labels_.end()) {
          return nullptr;
      }
      return it->second;
    });
}

const std::vector<instruction_block *> &instruction_block::blocks() const
{
    return blocks_;
}

bool instruction_block::walk_blocks(const instruction_block::block_predicate_visitor_callable &callable)
{
    std::stack<instruction_block*> block_stack {};
    block_stack.push(this);
    while (!block_stack.empty()) {
        auto block = block_stack.top();
        if (!callable(block)) {
            return false;
        }
        block_stack.pop();
        for (auto child_block : block->blocks()) {
            block_stack.push(child_block);
        }
    }
    return true;
}
void instruction_block::move_label_to_ireg_with_offset(registers_t dest_reg,
    const std::string &label_name, uint64_t offset)
{
    auto label_ref = make_unresolved_label_ref(label_name);

    instruction_t move_op;
    move_op.op = op_codes::move;
    move_op.size = op_sizes::qword;
    move_op.operands_count = 3;
    move_op.operands[0].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::reg;
    move_op.operands[0].value.r = dest_reg;
    move_op.operands[1].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant | operand_encoding_t::flags::unresolved;
    move_op.operands[1].value.u = label_ref->id;
    move_op.operands[2].type = operand_encoding_t::flags::integer
        | operand_encoding_t::flags::constant;
    move_op.operands[2].value.u = offset;
    make_block_entry(move_op);
}

// div variations
void instruction_block::div_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t dividend_reg, registers_t divisor_reg)
{
    make_div_instruction(size, dest_reg, dividend_reg, divisor_reg);
}

// mod variations
void instruction_block::mod_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t dividend_reg, registers_t divisor_reg)
{
    make_mod_instruction(size, dest_reg, dividend_reg, divisor_reg);
}

// swap variations
void instruction_block::swap_ireg_with_ireg(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    make_swap_instruction(size, dest_reg, src_reg);
}

// test mask for zero and branch
void instruction_block::test_mask_branch_if_zero(op_sizes size, registers_t value_reg,
                                                 registers_t mask_reg, registers_t address_reg)
{

}

// test mask for non-zero and branch
void instruction_block::test_mask_branch_if_not_zero(op_sizes size, registers_t value_reg,
                                                     registers_t mask_reg, registers_t address_reg)
{

}

///
void instruction_block::push(op_sizes sizes, registers_t reg)
{
    make_push_instruction(sizes, reg);
}

void instruction_block::pop(op_sizes size, registers_t reg)
{
    make_pop_instruction(size, reg);
}

/// load variations
void instruction_block::load_to_ireg(op_sizes size, registers_t dest_reg, registers_t address_reg, int64_t offset)
{
    make_load_instruction(size, dest_reg, address_reg, offset);
}

/// store
void instruction_block::store_from_ireg(op_sizes size, registers_t address_reg, registers_t src_reg, int64_t offset)
{
    make_store_instruction(size, address_reg, src_reg, offset);
}

// neg variations
void instruction_block::neg(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    make_neg_instruction(size, dest_reg, src_reg);
}

// or variations
void instruction_block::or_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_or_instruction(size, dest_reg, lhs_reg, rhs_reg);
}

// xor variations
void instruction_block::xor_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_xor_instruction(size, dest_reg, lhs_reg, rhs_reg);
}

// and variations
void instruction_block::and_ireg_by_ireg(op_sizes sizes, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_and_instruction(sizes, dest_reg, lhs_reg, rhs_reg);
}

// shl variations
void instruction_block::shl_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_shl_instruction(size, dest_reg, lhs_reg, rhs_reg);
}

// shr variations
void instruction_block::shr_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_shr_instruction(size, dest_reg, lhs_reg, rhs_reg);
}

// rol variations
void instruction_block::rol_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_rol_instruction(size, dest_reg, lhs_reg, rhs_reg);
}

// ror variations
void instruction_block::ror_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t lhs_reg, registers_t rhs_reg)
{
    make_ror_instruction(size, dest_reg, lhs_reg, rhs_reg);
}

/// move
void instruction_block::move_constant_to_ireg(op_sizes size, registers_t dest_reg, uint64_t immediate)
{
    make_move_instruction(size, dest_reg, immediate);
}


// inc variations
void instruction_block::inc(op_sizes size, registers_t reg)
{
    make_inc_instruction(size, reg);
}

// not variations
void instruction_block::not_op(op_sizes size, registers_t dest_reg, registers_t src_reg)
{
    make_not_instruction(size, dest_reg, src_reg);
}

// dec variations
void instruction_block::dec(op_sizes size, registers_t reg)
{
    make_dec_instruction(size, reg);
}

void instruction_block::mul_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t multiplicand_reg, registers_t multiplier_reg)
{
    make_mul_instruction(size, dest_reg, multiplicand_reg, multiplier_reg);
}

// add variations
void instruction_block::add_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t augend_reg, registers_t addened_reg)
{
    make_add_instruction(size, dest_reg, augend_reg, addened_reg);
}

// sub variations
void instruction_block::sub_ireg_by_ireg(op_sizes size, registers_t dest_reg, registers_t augend_reg, registers_t addened_reg)
{
    make_sub_instruction(size, dest_reg, augend_reg, addened_reg);
}

}