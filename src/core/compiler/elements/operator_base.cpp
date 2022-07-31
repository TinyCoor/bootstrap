//
// Created by 12132 on 2022/4/23.
//

#include "operator_base.h"
namespace gfx::compiler{
operator_base::operator_base(block* parent, element_type_t element_type, operator_type_t type)
	: element(parent, element_type),operator_type_(type)
{
}

operator_type_t operator_base::operator_type() const
{
	return operator_type_;
}

}