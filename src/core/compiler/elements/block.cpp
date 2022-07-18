//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
#include "vm/assembler.h"
#include "initializer.h"
#include "numeric_type.h"
#include "statement.h"
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

bool block::define_data(result &r, string_set_t& interned_strings, assembler &assembler)
{
	if (!identifiers_.empty()) {
        auto constant_init = identifiers_.constants(true);
        if (!constant_init.empty()) {
            auto section = assembler.segment(fmt::format("rodata_{}", id()),segment_type_t::constant);
            section->initialize(true);
            add_symbols(r, section, constant_init);
        }

        auto constant_uninit = identifiers_.constants(false);
        if (!constant_uninit.empty()) {
            auto section = assembler.segment(fmt::format("bss_{}", id()), segment_type_t::constant);
            add_symbols(r, section, constant_init);
        }

        auto global_init = identifiers_.globals(true);
        if (!global_init.empty()) {
            auto section = assembler.segment(fmt::format("data_{}", id()), segment_type_t::data);
            add_symbols(r, section, global_init);
        }

        auto global_uninit = identifiers_.globals(false);
        if (!global_uninit.empty()) {
            auto section = assembler.segment(fmt::format("bss_data_{}", id()), segment_type_t::data);
            add_symbols(r, section, global_uninit);
        }
	}
	return !r.is_failed();
}

void block::add_symbols(result& r, segment *segment, const identifier_list_t &list)
{
	for (auto& var: list) {
		switch (var->type()->element_type()) {
            case element_type_t::bool_type:
			case element_type_t::numeric_type: {
				auto symbol = segment->symbol(var->name(),
					integer_symbol_type_for_size(var->type()->size_in_bytes()));
                uint64_t value = 0;
                if (var->as_integer(value)) {
                    symbol->value(value);
                }
                break;
			}
            case element_type_t::any_type:
            case element_type_t::type_info:
            case element_type_t::array_type:
			case element_type_t::string_type:
			case element_type_t::composite_type: {
                auto size_in_bytes = var->type()->size_in_bytes();
                auto symbol = segment->symbol(var->name(), symbol_type_t::bytes, size_in_bytes);
                if (var->initializer() != nullptr){
                    symbol->pending_address_from_id(var->initializer()->id());
                }
                uint64_t value = 0;
                symbol->value(value);
                break;
            }
			default: {
				break;
			}

		}
	}
}

bool block::on_emit(result &r, assembler &assembler)
{
    auto instruction_block = assembler.make_implicit_block();
    instruction_block->make_label(fmt::format("implicit_block_{}", id()));
    assembler.push_block(instruction_block);

    for (auto stmt : statements_) {
        stmt->emit(r, assembler);
    }

    for (auto blk : blocks_) {
        blk->emit(r, assembler);
    }

    assembler.pop_block();
    return !r.is_failed();
}
}