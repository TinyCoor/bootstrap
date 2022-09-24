//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
#include "import.h"
#include "comment.h"
#include "vm/assembler.h"
#include "initializer.h"
#include "types/procedure_type.h"
#include "statement.h"
#include "module.h"
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

import_list_t& block::imports()
{
    return imports_;
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
    auto cleanup = false;
    switch (element_type()) {
        case element_type_t::block:{
            instruction_block = context.assembler->make_basic_block();
            instruction_block->memo();
            auto parent_ns = parent_element_as<compiler::namespace_element>();
            if (parent_ns != nullptr) {
                instruction_block->current_entry()->comment(fmt::format("namespace: {}", parent_ns->name()));
            }
            instruction_block->current_entry()->blank_lines(1);
            auto block_label = instruction_block->make_label(label_name());
            instruction_block->current_entry()->label(block_label);
            context.assembler->push_block(instruction_block);
            cleanup = true;
            break;
        }
        case element_type_t::module_block: {
            instruction_block = context.assembler->make_basic_block();
            instruction_block->memo();

            auto parent_module = parent_element_as<compiler::module>();
            if (parent_module != nullptr) {
                instruction_block->current_entry()->comment(fmt::format(
                    "module: {}",
                    parent_module->source_file()->path().string()));
            }
            instruction_block->current_entry()->blank_lines(1);

            auto block_label = instruction_block->make_label(label_name());
            instruction_block->current_entry()->label(block_label);

            context.assembler->push_block(instruction_block);
            break;
        }
        case element_type_t::proc_type_block:
        case element_type_t::proc_instance_block:
        default:
            return false;
    }

    for (auto stmt : statements_) {
        stmt->emit(r, context);
    }
    if (cleanup) {
        context.assembler->pop_block();
    }
    return !r.is_failed();
}

void block::on_owned_elements(element_list_t &list)
{
    for (auto element : types_.as_list()) {
        list.emplace_back(element);
    }

    for (auto element : blocks_) {
        list.emplace_back(element);
    }

    for (auto element : comments_) {
        list.emplace_back(element);
    }

    for (auto element : statements_) {
        list.emplace_back(element);
    }

    for (auto element : identifiers_.as_list()) {
        list.emplace_back(element);
    }

    for (auto element : imports_) {
        list.emplace_back(element);
    }
}
}