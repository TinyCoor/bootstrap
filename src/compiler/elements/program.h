//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_PROGRAM_H_
#define COMPILER_ELEMENTS_PROGRAM_H_
#include "block.h"
#include "parser/ast.h"
#include "../bytecode_emitter.h"
#include "vm/instruction_emitter.h"
namespace gfx::compiler {
class program : public element {
public:
	explicit program(terp* terp);

	~program() override;

	bool initialize(result& r, const ast_node_shared_ptr& root);

	compiler::block* block();

	bool compile(result& r, const ast_node_shared_ptr& root);

	bool run(result& r);

	const element_map_t& elements() const;

	element* find_element(id_t id);

	compiler::type* find_type(const std::string& name) const;

	compiler::type* find_type_for_identifier(const std::string& name);

protected:
	friend class directive;

	terp* terp();

private:
	void initialize_core_types();

	bool execute_directives(result& r);

	bool resolve_unknown_types(result& r);

	bool resolve_unknown_identifiers(result& r);

private:
	void add_composite_type_fields(result& r, composite_type* struct_type, const ast_node_shared_ptr& block);

	compiler::identifier* add_identifier_to_scope(result& r, const ast_node_shared_ptr& symbol,
		const ast_node_shared_ptr& rhs);

	void add_procedure_instance(result& r, compiler::procedure_type* proc_type, const ast_node_shared_ptr& node);
private:

	cast* make_cast(compiler::block* parent_scope, compiler::type* type, element* expr);

	alias* make_alias(compiler::block* parent_scope, element* expr);

	label* make_label(compiler::block* parent_scope, const std::string& name);

	field* make_field(compiler::block* parent_scope, compiler::identifier* identifier);

	if_element* make_if(compiler::block* parent_scope, element* predicate, element* true_branch,
		element* false_branch);

	comment* make_comment(compiler::block* parent_scope, comment_type_t type, const std::string& value);

	directive* make_directive(compiler::block* parent_scope, const std::string& name, element* expr);

	statement* make_statement(compiler::block* parent_scope, label_list_t labels, element* expr);

	boolean_literal* make_bool(compiler::block* parent_scope, bool value);

	float_literal* make_float(compiler::block* parent_scope, double value);

	integer_literal* make_integer(compiler::block* parent_scope, uint64_t value);

	string_literal* make_string(compiler::block* parent_scope, const std::string& value);

	array_type* make_array_type(compiler::block* parent_scope, compiler::type* entry_type, size_t size);

	initializer* make_initializer(compiler::block* parent_scope, element* expr);

	expression* make_expression(compiler::block* parent_scope, element* expr);

	identifier* make_identifier(compiler::block* parent_scope, const std::string& name, initializer* expr);

	unknown_type* make_unknown_type(compiler::block* parent_scope, const std::string& name,
		bool is_array, size_t array_size);

	numeric_type* make_numeric_type(compiler::block* parent_scope, const std::string& name, int64_t min, uint64_t max);

	namespace_element* make_namespace( compiler::block* parent_scope, element* expr);

	/// template<typename ... Args> make_types(Args &args) {}
	class block* make_block(compiler::block* parent_scope);

	class block* push_new_block();

	composite_type* make_enum_type(compiler::block* parent_scope);

	composite_type* make_struct_type(compiler::block* parent_scope);

	composite_type* make_union_type(compiler::block* parent_scope);

	string_type* make_string_type(compiler::block* parent_scope);

	any_type* make_any_type(compiler::block* parent_scope);

	attribute* make_attribute(compiler::block* parent_scope, const std::string& name, element* expr);

	unary_operator* make_unary_operator(compiler::block* parent_scope, operator_type_t type, element* rhs);

	binary_operator* make_binary_operator(compiler::block* parent_scope, operator_type_t type, element* lhs, element* rhs);

	procedure_instance* make_procedure_instance(compiler::block* parent_scope, type* procedure_type, compiler::block* scope);

	argument_list* make_argument_list(compiler::block* parent_scope);

	procedure_call* make_procedure_call(compiler::block* parent_scope, compiler::identifier* identifier,
		compiler::argument_list* args);

	procedure_type* make_procedure_type(compiler::block* parent_scope, compiler::block* block_scope);

	namespace_type* make_namespace_type(compiler::block* parent_scope);

	return_element* make_return(compiler::block* parent_scope);

	compiler::type* find_array_type(compiler::type* entry_type, size_t size) const;



private:

	element* evaluate(result& r, const ast_node_shared_ptr& node);

	class block* pop_scope();

	void remove_element(id_t id);

	class block* current_scope() const;

	void push_scope(class block* block);

	bool is_subtree_constant(const ast_node_shared_ptr& node);

	compiler::identifier* find_identifier(const ast_node_shared_ptr& node);

private:
	assembler assembler_;
	class terp* terp_ = nullptr;
	element_map_t elements_ {};
	compiler::block* block_ = nullptr;
	bytecode_emitter_options_t options_;
	std::stack<compiler::block*> scope_stack_ {};
	identifier_list_t identifiers_with_unknown_types_ {};

};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
