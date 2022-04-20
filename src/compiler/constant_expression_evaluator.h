//
// Created by 12132 on 2022/4/15.
//

#ifndef CONSTANT_EXPRESSION_EVALUATOR_H_
#define CONSTANT_EXPRESSION_EVALUATOR_H_
#include "scope.h"
namespace gfx {
class constant_expression_evaluator {
public:
	explicit constant_expression_evaluator(scope *scope);

	virtual ~constant_expression_evaluator();

	ast_node_shared_ptr evaluate(result &r, const ast_node_shared_ptr &node);

private:
	bool is_subtree_constant(const ast_node_shared_ptr& node);

private:
	scope *scope_;
};
}

#endif // CONSTANT_EXPRESSION_EVALUATOR_H_
