//
// Created by 12132 on 2022/3/30.
//

#include "compiler.h"
#include "src/lexer/lexer.h"
#include "parser/ast/ast_formatter.h"

namespace gfx {
compiler::compiler(size_t heap_size,size_t stack_size)
	: 	terp_(heap_size, stack_size), global_scope(nullptr, nullptr)
{

}

compiler::~compiler() {
}

bool compiler::initialize(result& r) {
	return terp_.initialize(r);
}

bool compiler::compile(result& r, std::istream& input)
{
	return compile_stream(r, input);
}

bool compiler::compile_stream(result& r, std::istream& input) {
	parser alpha_parser(input);
	auto program_node = alpha_parser.parse(r);
	if (program_node != nullptr) {

	}
	ast_formatter formatter(program_node);
	formatter.format();
	return !r.is_failed();
}

void compiler::build_scope_tree(result &r, scope *scope, const ast_node_shared_ptr &node)
{
	if (scope == nullptr || node == nullptr) {
		return;
	}

	for (auto& child_node : node->children) {
		if (child_node->type == ast_node_types_t::basic_block) {
			auto child_scope = scope->add_child_scope(child_node);
			build_scope_tree(r, child_scope, child_node);
		} else {
			// XXX: need to recurse down lhs and rhs nodes
		}
	}
}

}