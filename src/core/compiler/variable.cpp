//
// Created by 12132 on 2022/9/24.
//

#include "variable.h"
#include "elements/types/type.h"
#include "compiler/elements/identifier.h"
#include <fmt/format.h>
namespace gfx::compiler {


bool variable_register_t::reserve(assembler *assembler)
{
    if (integer) {
        allocated = assembler->allocate_reg(value.i);
    } else {
        allocated = assembler->allocate_reg(value.f);
    }
    return allocated;
}

void variable_register_t::release(assembler *assembler)
{
    if (!allocated) {
        return;
    }
    if (integer) {
        assembler->free_reg(value.i);
    } else {
        assembler->free_reg(value.f);
    }
    allocated = false;
}


bool variable_t::init(gfx::assembler *assembler, instruction_block *block)
{
    if (!live) {
        return false;
    }

    if (address_loaded) {
        return true;
    }

    if (usage == identifier_usage_t::heap) {
        if (!address_reg.reserve(assembler)) {
            return false;
        }

        if (address_offset != 0) {
            block->move_label_to_ireg_with_offset(address_reg.value.i, name, address_offset);
        } else {
            block->move_label_to_ireg(address_reg.value.i, name);
        }
        block->current_entry()->comment(fmt::format("identifier '{}' address (global)", name));
    }

    address_loaded = true;

    return true;
}

bool variable_t::read(assembler* assembler, instruction_block* block)
{
    if (!live) {
        return false;
    }
    if (!init(assembler, block)) {
        return false;
    }

    std::string type_name = "global";
    if (requires_read) {
        if (!value_reg.reserve(assembler)) {
            return false;
        }

        if (usage == identifier_usage_t::stack) {
            type_name = stack_frame_entry_type_name(frame_entry->type);
            block->load_to_ireg(op_sizes::qword, value_reg.value.i, i_registers_t::fp, frame_entry->offset);
        } else {
            block->load_to_ireg(op_size_for_byte_size(type->size_in_bytes()), value_reg.value.i,
                                address_reg.value.i);
            block->current_entry()->comment(fmt::format("load identifier '{}' value ({})", name, type_name));
        }
        requires_read = false;
    }

//    auto target_reg = assembler->current_target_register();
//    if (target_reg != nullptr && target_reg->reg.i != value_reg.i) {
//        block->move_ireg_to_ireg(target_reg->reg.i, value_reg.i);
//        block->current_entry()->comment("assign target register to value register");
//    }

    return true;
}

bool variable_t::write(assembler* assembler, instruction_block* block)
{
    auto target_reg = assembler->current_target_register();
    if (target_reg == nullptr) {
        return false;
    }
    block->store_from_ireg(op_size_for_byte_size(type->size_in_bytes()),
        address_reg.value.i, target_reg->reg.i, frame_entry != nullptr ? frame_entry->offset : 0);
    written = true;
    requires_read = true;
    return true;
}
void variable_t::make_live(assembler* assembler)
{
    if (live) {
        return;
    }
    live = true;
    address_loaded = false;
    requires_read = type->access_model() != type_access_model_t::pointer;
}

void variable_t::make_dormat(assembler* assembler)
{
    if (!live) {
        return;
    }

    live = false;
    requires_read = false;
    address_loaded = false;
    value_reg.release(assembler);
    address_reg.release(assembler);
}
}