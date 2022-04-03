//
// Created by 12132 on 2022/4/1.
//

#include "ast.h"
namespace gfx{
ast_node_shared_ptr ast_builder::pop_scope() {
	if (scope_stack_.empty())
		return nullptr;
	scope_stack_.pop();
	return scope_stack_.top();
}

ast_node_shared_ptr ast_builder::current_scope() const {
	if (scope_stack_.empty())
		return nullptr;
	auto top = scope_stack_.top();
	return top;
}

ast_node_shared_ptr ast_builder::push_scope(const ast_node_shared_ptr& node) {
	scope_stack_.push(node);
	return scope_stack_.top();
}

}