//
// Created by 12132 on 2022/4/23.
//

#include "expression.h"
namespace gfx::compiler {
expression::expression(element* parent, element* root)
	: element(parent, element_type_t::expression),root_(root)
{
}

element *expression::root()
{
	return root_;
}

compiler::type *expression::on_infer_type(const compiler::program *program)
{
	if (root_ == nullptr) {
		return nullptr;
	}
	return root_->infer_type(program);
}

}