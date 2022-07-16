//
// Created by 12132 on 2022/5/2.
//

#include "numeric_type.h"
namespace gfx::compiler {
numeric_type::numeric_type(element* parent, const std::string &name, int64_t min, uint64_t max)
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

type_list_t numeric_type::make_types(result& r, element* parent)
{
	type_list_t list {};
	for (const auto& it : s_types_map) {
		auto type = new numeric_type(parent, it.first, it.second.min, it.second.max);
		type->initialize(r);
		list.push_back(type);
	}
	return list;
}

bool numeric_type::on_initialize(result &r)
{
	auto it = s_types_map.find(name());
	if (it == s_types_map.end()) {
		return false;
	}
	size_in_bytes(it->second.size_in_bytes);
	return true;
}

}