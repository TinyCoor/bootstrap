//
// Created by 12132 on 2022/10/11.
//

#ifndef COMPILER_ELEMENTS_AST_EVALUATOR_H_
#define COMPILER_ELEMENTS_AST_EVALUATOR_H_
#include "element_builder.h"
#include "parser/ast.h"
namespace gfx::compiler {
class ast_evaluator {
public:
    ast_evaluator(element_builder* builder, compiler::program* program);

    element *evaluate(result &r, compiler::session &session,
        const ast_node_shared_ptr &node,
        element_type_t default_block_type = element_type_t::block);

    element *evaluate_in_scope(result &r,
        compiler::session &session,
        const ast_node_shared_ptr &node,
        compiler::block *scope,
        element_type_t default_block_type = element_type_t::block);

private:
    void apply_attributes(result &r, compiler::session &session, compiler::element *element,
        const ast_node_shared_ptr &node);

    void add_procedure_instance(result &r, compiler::session &session, compiler::procedure_type *proc_type,
        const ast_node_shared_ptr &node);

    void add_expression_to_scope(compiler::block *scope, compiler::element *expr);

    void add_composite_type_fields(result &r, compiler::session &session, compiler::composite_type *type,
        const ast_node_shared_ptr &block);

    compiler::block *add_namespaces_to_scope(result &r, compiler::session &session,
        const ast_node_shared_ptr &node, compiler::symbol_element *symbol, compiler::block *parent_scope);

    compiler::element *resolve_symbol_or_evaluate(result &r, compiler::session &session,
        const ast_node_shared_ptr &node);

    compiler::identifier *add_identifier_to_scope(result &r, compiler::session &session,
        compiler::symbol_element *symbol, type_find_result_t &find_type_result,
        const ast_node_shared_ptr &node, size_t source_index,
        compiler::block *parent_scope = nullptr);

private:
    element_builder *builder_;
    compiler::program* program_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_AST_EVALUATOR_H_
