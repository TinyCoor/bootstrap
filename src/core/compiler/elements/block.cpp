//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
#include "fmt/format.h"
#include "numeric_type.h"
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

bool block::emit(result &r, assembler &ass)
{
	return false;
}

bool block::define_data(result &r, assembler &assembler)
{
	if (identifiers_.empty()) {
		return true;
	}

	auto constant_init = identifiers_.constants(true);
	if (!constant_init.empty()) {
		auto section = assembler.segment(fmt::format("rodata_{}", id()),segment_type_t::constant,
			assembler.location_counter());
		section->initialized = true;
		add_symbols(r, section, constant_init);
	}

	auto constant_uninit = identifiers_.constants(false);
	if (!constant_uninit.empty()) {
		auto section = assembler.segment(fmt::format("bss_{}", id()), segment_type_t::constant,
			assembler.location_counter());
		add_symbols(r, section, constant_init);
	}

	auto global_init = identifiers_.globals(true);
	if (!global_init.empty()) {
		auto section = assembler.segment(fmt::format("data_{}", id()), segment_type_t::data,
			assembler.location_counter());
		add_symbols(r, section, global_init);
	}

	auto global_uninit = identifiers_.globals(false);
	if (!global_uninit.empty()) {
		auto section = assembler.segment(fmt::format("bss_data_{}", id()), segment_type_t::data,
			assembler.location_counter());
		add_symbols(r, section, global_uninit);
	}

	return !r.is_failed();
}

void block::add_symbols(result& r, segment_t *segment, const identifier_list_t &list)
{
	for (auto& var: list) {
		switch (var->type()->element_type()) {
			case element_type_t::numeric_type: {
				auto symbol = segment->symbol(var->name(),
					integer_symbol_type_for_size(var->type()->size_in_bytes()));
				symbol->value.int_value = 0;
				break;
			}
			case element_type_t::string_type: {
				break;
			}
			case element_type_t::array_type: {
				break;
			}
			case element_type_t::bool_type: {
				break;
			}
			case element_type_t::composite_type: {
				break;
			}
			default: {
				break;
			}

		}
	}
}

}