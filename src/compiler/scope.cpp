//
// Created by 12132 on 2022/4/9.
//

#include "scope.h"
namespace gfx {

scope::scope(scope *parent, const ast_node_shared_ptr &node) : parent_(parent), node_(node)
{
}

void scope::clear()
{
	children_.clear();
	symbol_table_.clear();
}
uint64_t scope::address() const
{
	return address_;
}

scope *scope::parent()
{
	return parent_;
}

void scope::address(uint64_t value)
{
	address_ = value;
}

ast_node_shared_ptr scope::ast_node()
{
	return node_;
}

const scope_list &scope::children() const
{
	return children_;
}

scope *scope::add_child_scope(const ast_node_shared_ptr &node)
{
	auto scope = std::make_unique<gfx::scope>(this, node);
	children_.push_back(std::move(scope));
	return scope.get();
}

}