//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_PROGRAM_H_
#define COMPILER_ELEMENTS_PROGRAM_H_
#include "block.h"
#include "parser/ast.h"
namespace gfx::compiler {
class program : public element {
public:
	program();
	~program() override;

	bool initialize(result& r, const ast_node_shared_ptr& root);

	compiler::block* block();

	const element_map_t& elements() const;

	element* find_element(id_t id);

private:

	/// template<typename ... Args> make_types(Args &args) {}
	class block* make_block(compiler::block* parent_scope = nullptr);

	class block* push_new_block();

	any_type* make_any_type();

	void add_enum_fields(result& r, composite_type* enum_type, const ast_node_shared_ptr& block);

	void add_struct_fields(result& r, composite_type* struct_type, const ast_node_shared_ptr& block);

	void add_union_fields(result& r, composite_type* union_type, const ast_node_shared_ptr& block);

	void add_procedure_instance(result& r, procedure_type* proc_type,const ast_node_shared_ptr& node);

	field* make_field(compiler::identifier* identifier);

	composite_type* make_enum_type();

	composite_type* make_struct_type();

	composite_type* make_union_type();

	string_type* make_string_type();

	alias* make_alias(element* expr);

	label* make_label(const std::string& name);

	numeric_type* make_numeric_type(const std::string& name, int64_t min, uint64_t max);

	attribute* make_attribute(const std::string& name, element* expr);

	comment* make_comment(comment_type_t type, const std::string& value);

	directive* make_directive(const std::string& name, element* expr);

	statement* make_statement(label_list_t labels, element* expr);

	identifier* make_identifier(const std::string& name, initializer* expr,
								compiler::block* block_scope = nullptr);

	unary_operator* make_unary_operator(operator_type_t type, element* rhs);

	binary_operator* make_binary_operator(operator_type_t type, element* lhs, element* rhs);

	expression* make_expression(element* expr);

	initializer* make_initializer(element* expr, class block* block_scope = nullptr);

	procedure_instance* make_procedure_instance(type* procedure_type, class block* scope);

	procedure_call* make_procedure_call(type* procedure_type, element* expr);

	procedure_type* make_procedure_type(compiler::block* block_scope = nullptr);

	cast* make_cast(compiler::type* type, element* expr);

	if_element* make_if(element* predicate, element* true_branch, element* false_branch);

	return_element* make_return();

	compiler::identifier* add_identifier_to_scope(result& r, const ast_node_shared_ptr& symbol,
		const ast_node_shared_ptr& rhs);

	namespace_type* make_namespace_type();

	namespace_element* make_namespace(element* expr, compiler::block* block_scope = nullptr);

	boolean_literal* make_bool(bool value);

	float_literal* make_float(double value);

	integer_literal* make_integer(uint64_t value);

	string_literal* make_string(const std::string& value);

	array_type* make_array_type(compiler::type* entry_type, size_t size);

	compiler::type* find_array_type(compiler::type* entry_type, size_t size);


private:

	element* evaluate(result& r, const ast_node_shared_ptr& node);

	class block* pop_scope();

	void initialize_core_types();

	class block* current_scope() const;

	void push_scope(class block* block);

	bool is_subtree_constant(const ast_node_shared_ptr& node);

	type* find_type(const std::string& name);

	compiler::identifier* find_identifier(const ast_node_shared_ptr& node);


private:
	element_map_t elements_ {};
	compiler::block* block_ = nullptr;
	std::stack<compiler::block*> scope_stack_ {};
};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
