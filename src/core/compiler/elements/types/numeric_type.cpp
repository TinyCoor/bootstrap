//
// Created by 12132 on 2022/5/2.
//

#include "numeric_type.h"
#include "../symbol_element.h"
#include "core/compiler/elements/program.h"
namespace gfx::compiler {
numeric_type::numeric_type(block* parent, symbol_element* symbol,  int64_t min, uint64_t max,
                           bool is_signed)
	: type(parent, element_type_t::numeric_type, symbol), min_(min), max_(max), is_signed_(is_signed)
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
	for (const auto& props : s_type_properties) {
		auto type = program->make_numeric_type(r, parent, props.name, props.min, props.max, props.is_signed);
		type->initialize(r, program);
		program->add_type_to_scope(type);
	}
	return list;
}

bool numeric_type::on_initialize(result &r, compiler::program* program)
{
	auto it = s_types_map.find(symbol()->name());
	if (it == s_types_map.end()) {
		return false;
	}
	size_in_bytes(it->second->size_in_bytes);
	return true;
}

std::string numeric_type::narrow_to_value(uint64_t value)
{
    for (const auto& props : s_type_properties) {
        if (value >= props.min && value <= props.max) {
            return props.name;
        }
    }
    return "u32";
}

bool numeric_type::is_signed() const
{
    return is_signed_;
}


}