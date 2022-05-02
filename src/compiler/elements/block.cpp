//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
namespace gfx::compiler {
block::block(block* parent) : parent_(parent)
{
}

block::~block()
{
}

block* block::parent() const
{
	return parent_;
}

element_list_t& block::children()
{
	return children_;
}

bool block::remove_type(const std::string& name) {
	return types_.erase(name) > 0;
}

bool block::remove_identifier(const std::string& name)
{
	return identifiers_.erase(name) > 0;
}

compiler::type* block::find_type(const std::string& name)
{
	auto it = types_.find(name);
	if (it != types_.end()) {
		return it->second;
	}
	return nullptr;
}

compiler::identifier* block::find_identifier(const std::string& name)
{
	auto it = identifiers_.find(name);
	if (it != identifiers_.end()) {
		return it->second;
	}
	return nullptr;
}

}