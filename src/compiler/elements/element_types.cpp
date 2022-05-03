//
// Created by 12132 on 2022/5/3.
//

#include "element_types.h"
#include "attribute.h"

namespace gfx::compiler {
attribute_map_t::attribute_map_t(element *parent)
	: parent_(parent)
{

}

attribute_map_t::~attribute_map_t()
{
	for (auto attr : attrs_)
		delete attr.second;
	attrs_.clear();
}

bool attribute_map_t::remove(const std::string &name)
{
	return attrs_.erase(name) > 0;
}
compiler::attribute *attribute_map_t::find(const std::string &name)
{
	auto it = attrs_.find(name);
	if (it != attrs_.end()) {
		return it->second;
	}

	return nullptr;
}
element * attribute_map_t::add(const std::string &name, element *expr)
{
	auto attr = new compiler::attribute(parent_, name, expr);
	attrs_.insert(std::make_pair(name, attr));
	return attr;
}
}