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
#include <common/defer.h>
#include "core/compiler/session.h"

namespace gfx::compiler {
block::block(compiler::module* module, block* parent, element_type_t type)
	: element(module, parent, type)
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

bool block::on_emit(compiler::session& session)
{
    auto& assembler = session.assembler();
    instruction_block* instruction_block = nullptr;
    auto clean_up = false;
    defer({
          if (clean_up) {
              assembler.pop_block();
          }
    });
    switch (element_type()) {
        case element_type_t::block:{
            instruction_block = assembler.make_basic_block();
            instruction_block->memo();
            auto parent_ns = parent_element_as<compiler::namespace_element>();
            if (parent_ns != nullptr) {
                instruction_block->current_entry()->comment(
                    fmt::format("namespace: {}", parent_ns->name()),
                    session.emit_context().indent);
            }
            instruction_block->current_entry()->blank_lines(1);
            auto block_label = assembler.make_label(label_name());
            instruction_block->current_entry()->label(block_label);
            assembler.push_block(instruction_block);
            clean_up = true;
            break;
        }
        case element_type_t::module_block: {
            instruction_block = assembler.make_basic_block();
            instruction_block->memo();

            auto parent_module = parent_element_as<compiler::module>();
            if (parent_module != nullptr) {
                instruction_block->current_entry()->comment(
                    fmt::format("module: {}", parent_module->source_file()->path().string()),
                    session.emit_context().indent);
                clean_up = !parent_module->is_root();
            }
            instruction_block->current_entry()->blank_lines(1);

            auto block_label = assembler.make_label(label_name());
            instruction_block->current_entry()->label(block_label);

            assembler.push_block(instruction_block);
            break;
        }
        case element_type_t::proc_type_block:
        case element_type_t::proc_instance_block:
        default:
            return false;
    }

    for (auto stmt : statements_) {
        stmt->emit(session);
    }
    return !session.result().is_failed();
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