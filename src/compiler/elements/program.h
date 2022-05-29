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
private:
	void initialize_core_types();

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

	array_type* make_array_type(  compiler::block* parent_scope, compiler::type* entry_type, size_t size);

	initializer* make_initializer(compiler::block* parent_scope, element* expr);

	expression* make_expression(compiler::block* parent_scope, element* expr);

	identifier* make_identifier(const std::string& name, initializer* expr,
		compiler::block* block_scope = nullptr);

	unknown_type* make_unknown_type(compiler::block* parent_scope, const std::string& name,
		bool is_array, size_t array_size);

	/// template<typename ... Args> make_types(Args &args) {}
	class block* make_block(compiler::block* parent_scope = nullptr);

	class block* push_new_block();

	any_type* make_any_type();

	void add_enum_fields(result& r, composite_type* enum_type, const ast_node_shared_ptr& block);

	void add_struct_fields(result& r, composite_type* struct_type, const ast_node_shared_ptr& block);

	void add_union_fields(result& r, composite_type* union_type, const ast_node_shared_ptr& block);

	void add_procedure_instance(result& r, procedure_type* proc_type,const ast_node_shared_ptr& node);


	composite_type* make_enum_type();

	composite_type* make_struct_type();

	composite_type* make_union_type();

	string_type* make_string_type();


	numeric_type* make_numeric_type(const std::string& name, int64_t min, uint64_t max);

	attribute* make_attribute(const std::string& name, element* expr);





	unary_operator* make_unary_operator(operator_type_t type, element* rhs);

	binary_operator* make_binary_operator(operator_type_t type, element* lhs, element* rhs);

	expression* make_expression(element* expr);



	procedure_instance* make_procedure_instance(type* procedure_type, class block* scope);

	argument_list* make_argument_list();

	procedure_call* make_procedure_call(compiler::identifier* identifier,
		compiler::argument_list* args);

	procedure_type* make_procedure_type(compiler::block* block_scope = nullptr);



	return_element* make_return();

	compiler::identifier* add_identifier_to_scope(result& r, const ast_node_shared_ptr& symbol,
		const ast_node_shared_ptr& rhs);

	namespace_type* make_namespace_type();

	namespace_element* make_namespace(element* expr, compiler::block* block_scope = nullptr);



	compiler::type* find_array_type(compiler::type* entry_type, size_t size) const;

	void resolve_pending_type_inference();

private:

	element* evaluate(result& r, const ast_node_shared_ptr& node);

	class block* pop_scope();



	class block* current_scope() const;

	void push_scope(class block* block);

	bool is_subtree_constant(const ast_node_shared_ptr& node);

	compiler::identifier* find_identifier(const ast_node_shared_ptr& node);

private:
	assembler assembler_;
	terp* terp_ = nullptr;
	element_map_t elements_ {};
	compiler::block* block_ = nullptr;
	bytecode_emitter_options_t options_;
	std::stack<compiler::block*> scope_stack_ {};
	identifier_list_t identifiers_pending_type_inference_ {};

};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
