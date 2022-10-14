//
// Created by 12132 on 2022/9/24.
//

#include "variable.h"
#include "emit_context.h"
#include "elements/types/type.h"
#include "compiler/elements/identifier.h"
#include <fmt/format.h>
namespace gfx::compiler {
bool variable_register_t::reserve(assembler *assembler)
{
    allocated = assembler->allocate_reg(reg);
    return allocated;
}

void variable_register_t::release(assembler *assembler)
{
    if (!allocated) {
        return;
    }
    assembler->free_reg(reg);
    allocated = false;
}


bool variable_t::init(emit_context_t& context, instruction_block *block)
{
    if (!live) {
        return false;
    }

    if (address_loaded) {
        return true;
    }

    if (usage == identifier_usage_t::heap) {
        if (!address_reg.reserve(context.assembler)) {
            return false;
        }

        if (address_offset != 0) {
            block->move_label_to_reg_with_offset(address_reg.reg, name, address_offset);
        } else {
            block->move_label_to_reg(address_reg.reg, name);
        }
        block->current_entry()->blank_lines(1)->comment(fmt::format("identifier '{}' address (global)", name), context.indent);
    }

    value_reg.reg.type = register_type_t::integer;
    if (type != nullptr &&  type->access_model() == type_access_model_t::value) {
        value_reg.reg.size = op_size_for_byte_size(type->size_in_bytes());
        if (type->number_class() == type_number_class_t::floating_point) {
            value_reg.reg.type = register_type_t::floating_point;
        }
    }
    address_loaded = true;

    return true;
}

bool variable_t::read(emit_context_t& context, instruction_block* block)
{
    if (!live) {
        return false;
    }
    if (!init(context, block)) {
        return false;
    }

    std::string type_name = "global";
    if (requires_read) {
        if (!value_reg.reserve(context.assembler)) {
            return false;
        }

        if (usage == identifier_usage_t::stack) {
            type_name = stack_frame_entry_type_name(frame_entry->type);
            block->load_to_reg(value_reg.reg, register_t::fp(), frame_entry->offset);
        } else {
            block->load_to_reg(value_reg.reg, address_reg.reg);
            block->current_entry() ->blank_lines(1)->comment(fmt::format("load identifier '{}' value ({})", name, type_name), context.indent);
        }
        requires_read = false;
    }

    return true;
}

bool variable_t::write(emit_context_t& context, instruction_block* block)
{
    auto target_reg =context.assembler->current_target_register();
    if (target_reg == nullptr) {
        return false;
    }
    block->store_from_reg(address_reg.reg, *target_reg, frame_entry != nullptr ? frame_entry->offset : 0);
    written = true;
    requires_read = true;
    return true;
}

void variable_t::make_live(emit_context_t& context)
{
    if (live) {
        return;
    }
    live = true;
    address_loaded = false;
    requires_read = type->access_model() != type_access_model_t::pointer;
}

void variable_t::make_dormat(emit_context_t& context)
{
    if (!live) {
        return;
    }

    live = false;
    requires_read = false;
    address_loaded = false;
    value_reg.release(context.assembler);
    address_reg.release(context.assembler);
}
}