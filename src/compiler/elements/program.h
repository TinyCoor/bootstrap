//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_PROGRAM_H_
#define COMPILER_ELEMENTS_PROGRAM_H_
#include "block.h"
#include "parser/ast.h"
namespace gfx::compiler {
class program : public block {
public:
	program();
	~program() override;

	bool initialize(result& r, const ast_node_shared_ptr& root);

	element* find_element(id_t id);

private:

	block* make_new_block();

	any_type* make_any_type();

	string_type* make_string_type();

	numeric_type* make_numeric_type(const std::string& name, int64_t min, uint64_t max);

	field* make_field(const std::string& name, compiler::type* type, compiler::initializer* initializer);

	attribute* make_attribute(const std::string& name, element* expr);

	composite_type* make_enum();

	composite_type* make_union();

	composite_type* make_struct();

	element* evaluate(result& r, const ast_node_shared_ptr& node);

	block* pop_scope();

	void initialize_core_types();

	block* current_scope() const;

	void push_scope(block* block);

	bool is_subtree_constant(const ast_node_shared_ptr& node);

	identifier* make_identifier(element* expr);

	directive* make_directive(const std::string& name);

	line_comment* make_line_comment(const std::string& value);

private:
	std::stack<block*> scope_stack_ {};
	std::unordered_map<id_t, element*> elements_ {};
};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
