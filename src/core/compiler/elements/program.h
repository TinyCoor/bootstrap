//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_PROGRAM_H_
#define COMPILER_ELEMENTS_PROGRAM_H_
#include "block.h"
#include "element.h"
#include "parser/ast.h"
#include "../element_map.h"
#include "element_types.h"
#include "compiler/compiler_types.h"
#include "common/source_file.h"

namespace gfx::compiler {
struct type_find_result_t {
    qualified_symbol_t type_name{};
    bool is_array{false};
    size_t array_size{0};
    compiler::type* type = nullptr;
};

class program : public element {
public:
    using block_visitor_callable = std::function<bool (compiler::block*)>;

    program(terp* terp, assembler* assembler);

	~program() override;

	compiler::block *block() const;

	bool compile(result& r, compiler::session& session);

    void error(result& r, compiler::session& session, const std::string &code,
        const std::string &message, const source_location& location);

    void error(result& r, compiler::element *element, const std::string &code,
         const std::string &message, const source_location& location);


	module* compile_module(result& r, compiler::session& session, source_file *source);

	bool run(result& r);

    element_map& elements();

    compiler::type* find_type(const qualified_symbol_t& symbol) const;

    void disassemble(FILE *file);

protected:
	terp* terp();

    compiler::block* current_top_level();
private:
    bool on_emit(result& r, emit_context_t& context) override;

	void initialize_core_types(result &r);

    bool resolve_unknown_types(result& r);

    bool resolve_unknown_identifiers(result& r);

    bool visit_blocks(result& r, const block_visitor_callable& callable,
        compiler::block* root_block = nullptr);

private:
	void add_composite_type_fields(result& r, compiler::session& session, composite_type* struct_type, const ast_node_shared_ptr& block);

    void apply_attributes(result& r, compiler::session& session, compiler::element* element, const ast_node_shared_ptr& node);

	compiler::identifier* add_identifier_to_scope(result& r, compiler::session& session, symbol_element* symbol, type_find_result_t& find_type_result,
		const ast_node_shared_ptr& node, compiler::block* parent_scope = nullptr);

    compiler::symbol_element* make_symbol_from_node(result& r, const ast_node_shared_ptr& node);

    void add_procedure_instance(result& r, compiler::session& session, compiler::procedure_type* proc_type, const ast_node_shared_ptr& node);

    static void add_expression_to_scope(compiler::block* scope, compiler::element* expr);

    void add_type_to_scope(compiler::type* value);

    compiler::element* resolve_symbol_or_evaluate(result& r, compiler::session& session,
        const ast_node_shared_ptr& node);

    bool within_procedure_scope(compiler::block* parent_scope) const;

    static compiler::module* find_module(compiler::element* module);
private:
    friend class any_type;
    friend class directive;
    friend class tuple_type;
    friend class type_info;
    friend class array_type;
    friend class module_type;
    friend class string_type;
    friend class numeric_type;
    friend class unary_operator;
    friend class namespace_type;
    friend class procedure_type;
    friend class binary_operator;

    compiler::symbol_element* make_symbol(compiler::block* parent_scope,
        const std::string& name, const string_list_t& namespaces = {});

    static void make_qualified_symbol(qualified_symbol_t& symbol, const ast_node_shared_ptr& node);

    cast* make_cast(compiler::block* parent_scope, compiler::type* type, element* expr);

	alias* make_alias(compiler::block* parent_scope, element* expr);

	import* make_import(compiler::block* parent_scope, element* expr, element* from_expr);

	label* make_label(compiler::block* parent_scope, const std::string& name);

	field* make_field(compiler::block* parent_scope, compiler::identifier* identifier);

    module* make_module(compiler::block* parent_scope, compiler::block* scope);

    module_reference* make_module_reference(compiler::block* parent_scope, compiler::element* expr);

	if_element* make_if(compiler::block* parent_scope, element* predicate, element* true_branch,
		element* false_branch);

	comment* make_comment(compiler::block* parent_scope, comment_type_t type, const std::string& value);

	directive* make_directive(compiler::block* parent_scope, const std::string& name, element* expr);

	statement* make_statement(compiler::block* parent_scope, const label_list_t &labels, element* expr);

	boolean_literal* make_bool(compiler::block* parent_scope, bool value);

	float_literal* make_float(compiler::block* parent_scope, double value);

	integer_literal* make_integer(compiler::block* parent_scope, uint64_t value);

	string_literal* make_string(compiler::block* parent_scope, const std::string& value);

	initializer* make_initializer(compiler::block* parent_scope, element* expr);

	expression* make_expression(compiler::block* parent_scope, element* expr);

	identifier* make_identifier(compiler::block* parent_scope,
        symbol_element* symbol, initializer* expr);

	namespace_element* make_namespace(compiler::block* parent_scope, element* expr);

    template<typename T, typename Result, typename ... Args>
    auto make_type(Result r, Args&& ...args) -> decltype(new T(std::forward<Args>(args)...))
    {
        auto type = new T(std::forward<Args>(args)...);
        if (!type->initialize(r, this)) {
            return nullptr;
        }
        elements_.add(type);
        return type;
    }

	class block* make_block(compiler::block* parent_scope, element_type_t type);

	class block* push_new_block(element_type_t type = element_type_t::block);

	unknown_type* make_unknown_type(result &r, compiler::block* parent_scope, compiler::symbol_element* symbol,
        bool is_array, size_t array_size);

	numeric_type* make_numeric_type(result &r, compiler::block* parent_scope, const std::string& name,
		int64_t min, uint64_t max);

	array_type* make_array_type(result &r, compiler::block* parent_scope,
        compiler::type* entry_type, size_t size, compiler::block* scope);

	composite_type* make_enum_type(result &r, compiler::block* parent_scope, compiler::block* scope);

	composite_type* make_struct_type(result &r, compiler::block* parent_scope, compiler::block* scope);

	composite_type* make_union_type(result &r, compiler::block* parent_scope, compiler::block* scope);

	string_type* make_string_type(result& r, compiler::block* parent_scope, compiler::block* scope);

    type_info* make_type_info_type(result& r, compiler::block* parent_scope, compiler::block* scope);

    tuple_type* make_tuple_type(result& r, compiler::block* parent_scope, compiler::block* scope);

    module_type* make_module_type(result& r, compiler::block* parent_scope, compiler::block* scope);

    any_type* make_any_type(result&r, compiler::block* parent_scope, compiler::block* scope);

	procedure_type* make_procedure_type(result& r, compiler::block* parent_scope, compiler::block* block_scope);

	namespace_type* make_namespace_type(result &r, compiler::block* parent_scope);

	attribute* make_attribute(compiler::block* parent_scope, const std::string& name, element* expr);

	unary_operator* make_unary_operator(compiler::block* parent_scope, operator_type_t type, element* rhs);

	binary_operator* make_binary_operator(compiler::block* parent_scope, operator_type_t type, element* lhs, element* rhs);

	procedure_instance* make_procedure_instance(compiler::block* parent_scope, type* procedure_type, compiler::block* scope);

	argument_list* make_argument_list(compiler::block* parent_scope);

    identifier_reference* make_identifier_reference(compiler::block* parent_scope,
        const qualified_symbol_t& symbol, compiler::identifier* identifier);

	procedure_call* make_procedure_call(compiler::block* parent_scope, compiler::identifier_reference* reference,
		compiler::argument_list* args);

	return_element* make_return(compiler::block* parent_scope);

	compiler::type* find_array_type(compiler::type* entry_type, size_t size) const;

    unknown_type* unknown_type_from_result(result& r, compiler::block* scope, compiler::identifier* identifier,
       type_find_result_t& result);
private:
    element* evaluate(result& r, compiler::session& session, const ast_node_shared_ptr& node,
		element_type_t default_block_type = element_type_t::block);

    element* evaluate_in_scope(result& r, compiler::session& session, const ast_node_shared_ptr& node,
        compiler::block* scope, element_type_t default_block_type = element_type_t::block);

    bool find_identifier_type(result& r, type_find_result_t& result, const ast_node_shared_ptr& type_node,
        compiler::block* parent_scope = nullptr);

	class block* pop_scope();

	class block* current_scope() const;

	void push_scope(class block* block);

	compiler::identifier* find_identifier(const qualified_symbol_t& symbol);

private:
	assembler* assembler_ = nullptr;
	class terp* terp_ = nullptr;
	element_map elements_ {};
	compiler::block* block_ = nullptr;
    std::stack<compiler::block*> top_level_stack_;
	std::stack<compiler::block*> scope_stack_ {};
	identifier_list_t identifiers_with_unknown_types_ {};
    identifier_reference_list_t unresolved_identifier_references_ {};
    std::unordered_map<std::string, string_literal_list_t> interned_string_literals_ {};
};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
