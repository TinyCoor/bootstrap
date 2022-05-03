//
// Created by 12132 on 2022/4/23.
//

#include "expression.h"
namespace gfx::compiler {
expression::expression(element* parent, element* root)
	: element(parent),root_(root)
{
}

expression::~expression()
{
}

}