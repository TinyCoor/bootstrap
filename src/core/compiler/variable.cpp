//
// Created by 12132 on 2022/9/24.
//

#include "variable.h"
#include "elements/types/type.h"
#include "compiler/elements/identifier.h"
#include <fmt/format.h>
namespace gfx::compiler {
bool variable_t::init(gfx::assembler *assembler, instruction_block *block)
{
    if (address_loaded) {
        return true;
    }

    if (usage == identifier_usage_t::heap) {
        if (address_offset != 0) {
            block->move_label_to_ireg_with_offset(address_reg, name, address_offset);
        } else {
            block->move_label_to_ireg(address_reg, name);
        }
        block->current_entry()->comment(fmt::format("identifier '{}' address (global)", name));
    }

    address_loaded = true;

    return true;
}

bool variable_t::read(assembler* assembler, instruction_block* block)
{
    init(assembler, block);

    std::string type_name = "global";
    if (requires_read) {
        if (usage == identifier_usage_t::stack) {
            type_name = stack_frame_entry_type_name(frame_entry->type);
            block->load_to_ireg(op_sizes::qword, value_reg.i, i_registers_t::fp, frame_entry->offset);
        } else {
            block->load_to_ireg(op_size_for_byte_size(type->size_in_bytes()), value_reg.i,address_reg);
            block->current_entry()->comment(fmt::format("load identifier '{}' value ({})", name, type_name));
        }
        requires_read = false;
    }

    auto target_reg = assembler->current_target_register();
    if (target_reg != nullptr && target_reg->reg.i != value_reg.i) {
        block->move_ireg_to_ireg(target_reg->reg.i, value_reg.i);
        block->current_entry()->comment("assign target register to value register");
    }

    return true;
}

bool variable_t::write(assembler* assembler, instruction_block* block)
{
    auto target_reg = assembler->current_target_register();
    if (target_reg == nullptr) {
        return false;
    }
    block->store_from_ireg(op_size_for_byte_size(type->size_in_bytes()),
        address_reg, target_reg->reg.i, frame_entry != nullptr ? frame_entry->offset : 0);
    written = true;
    requires_read = true;
    return true;
}

}