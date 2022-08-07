//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
#include "vm/assembler.h"
#include "initializer.h"
#include "core/compiler/elements/types/procedure_type.h"
#include "statement.h"
#include "string_literal.h"
#include "namespace_element.h"
#include <fmt/format.h>

namespace gfx::compiler {
block::block(block* parent, element_type_t type)
	: element(parent, type)
{
}

block::~block() = default;

type_map_t& block::types()
{
	return types_;
}

identifier_map_t& block::identifiers()
{
	return identifiers_;
}

statement_list_t &block::statements()
{
	return statements_;
}

comment_list_t &block::comments()
{
	return comments_;
}

block_list_t &block::blocks()
{
	return blocks_;
}

bool block::on_emit(result &r, emit_context_t& context)
{
    instruction_block* instruction_block = nullptr;

    switch (element_type()) {
        case element_type_t::block:{
            instruction_block = context.assembler->make_basic_block();
            instruction_block->memo();
            auto parent_ns = parent_element_as<compiler::namespace_element>();
            if (parent_ns != nullptr) {
                auto current_entry = instruction_block->current_entry();
                current_entry->comment(fmt::format("namespace: {}", parent_ns->name()));
                current_entry->blank_lines(1);
            }
            auto block_label = instruction_block->make_label(label_name());
            instruction_block->current_entry()->label(block_label);
            context.assembler->push_block(instruction_block);
            break;
        }
        case element_type_t::proc_type_block: {
            instruction_block = context.assembler->current_block();
            break;
        }
        case element_type_t::proc_instance_block:
            instruction_block =context.assembler->current_block();
            break;
        default:
            return false;
    }

    for (auto stmt : statements_) {
        stmt->emit(r, context);
    }

    auto block_data = context.top<block_data_t>();
    if (block_data == nullptr || block_data->recurse) {
        for (auto blk : blocks_) {
            blk->emit(r, context);
        }
    }

    if (element_type() == element_type_t::block) {
       context.assembler->pop_block();
    }

    return !r.is_failed();
}
}