//
// Created by 12132 on 2022/4/23.
//

#include "operator_base.h"
namespace gfx::compiler{
operator_base::operator_base(operator_type_t type)
	: type_(type)
{
}

operator_base::~operator_base()
{

}

operator_type_t operator_base::type() const
{
	return type_;
}

}