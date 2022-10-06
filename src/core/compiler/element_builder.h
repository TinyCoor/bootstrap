//
// Created by 12132 on 2022/10/6.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_ELEMENT_BUILDER_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_ELEMENT_BUILDER_H_
#include "elements/element.h"
#include "element_map.h"

namespace gfx::compiler {
class program;
class element_builder {
public:
    element_builder(program* program);
    ~element_builder();

    compiler::cast* make_cast(compiler::block* parent_scope, compiler::type* type, element* expr);

    compiler::alias* make_alias(compiler::block* parent_scope, element* expr);

    compiler::import* make_import(compiler::block* parent_scope, element* expr,
         element* from_expr, compiler::module* module);

    compiler::label* make_label(compiler::block* parent_scope, const std::string& name);

    compiler::field* make_field(compiler::block* parent_scope, compiler::identifier* identifier);

    compiler::module* make_module(compiler::block* parent_scope, compiler::block* scope);

    compiler::module_reference* make_module_reference(compiler::block* parent_scope, compiler::element* expr);

    compiler::if_element* make_if(compiler::block* parent_scope, element* predicate, element* true_branch,
        element* false_branch);

    compiler::comment* make_comment(compiler::block* parent_scope, comment_type_t type, const std::string& value);

    compiler::directive* make_directive(compiler::block* parent_scope, const std::string& name, element* expr);

    compiler::statement* make_statement(compiler::block* parent_scope, const label_list_t &labels, element* expr);

    compiler::boolean_literal* make_bool(compiler::block* parent_scope, bool value);

    compiler::float_literal* make_float(compiler::block* parent_scope, double value);

    compiler::integer_literal* make_integer(compiler::block* parent_scope, uint64_t value);

    compiler::string_literal* make_string(compiler::block* parent_scope, const std::string& value);

    compiler::initializer* make_initializer(compiler::block* parent_scope, element* expr);

    compiler::expression* make_expression(compiler::block* parent_scope, element* expr);

    compiler::identifier* make_identifier(compiler::block* parent_scope,
        compiler::symbol_element* symbol, initializer* expr);

    compiler::namespace_element* make_namespace(compiler::block* parent_scope, compiler::element* expr);

    compiler::symbol_element* make_symbol(compiler::block* parent_scope,
        const std::string& name, const string_list_t& namespaces = {});

    class compiler::block* make_block(compiler::block* parent_scope, element_type_t type);

    compiler::bool_type* make_bool_type(result& r, compiler::block* parent_scope);

    compiler::unknown_type* make_unknown_type(result &r, compiler::block* parent_scope, compiler::symbol_element* symbol,
        bool is_pointer, bool is_array, size_t array_size);

    compiler::numeric_type* make_numeric_type(result &r, compiler::block* parent_scope, const std::string& name,
         int64_t min, uint64_t max, bool is_signed, type_number_class_t number_class);

    compiler::array_type* make_array_type(result &r, compiler::block* parent_scope, compiler::block* scope,
                                          compiler::type* entry_type, size_t size);

    compiler::pointer_type* make_pointer_type(result& r, compiler::block* parent_scope,
                                              compiler::type* base_type);

    compiler::composite_type* make_enum_type(result &r, compiler::block* parent_scope, compiler::block* scope);

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

private:

    compiler::program* program_;
};
}

#endif //BOOTSTRAP_SRC_CORE_COMPILER_ELEMENT_BUILDER_H_
