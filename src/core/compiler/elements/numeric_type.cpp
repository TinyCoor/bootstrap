//
// Created by 12132 on 2022/5/2.
//

#include "numeric_type.h"
#include "program.h"
namespace gfx::compiler {
numeric_type::numeric_type(block* parent, const std::string &name, int64_t min, uint64_t max)
	: type(parent, element_type_t::numeric_type, name), min_(min), max_(max)
{

}

int64_t numeric_type::min() const
{
	return min_;
}

uint64_t numeric_type::max() const
{
	return max_;
}

symbol_type_t numeric_type::symbol_type() const
{
	return symbol_type_t::u64;
}

type_list_t numeric_type::make_types(result& r, compiler::block* parent, compiler::program* program)
{
	type_list_t list {};
	for (const auto& it : s_types_map) {
		auto type = program->make_numeric_type(r, parent, it.first, it.second.min, it.second.max);
		type->initialize(r, program);
		program->add_type_to_scope(type);
	}
	return list;
}

bool numeric_type::on_initialize(result &r, compiler::program* program)
{
	auto it = s_types_map.find(name());
	if (it == s_types_map.end()) {
		return false;
	}
	size_in_bytes(it->second.size_in_bytes);
	return true;
}

}