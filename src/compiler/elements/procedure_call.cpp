//
// Created by 12132 on 2022/5/14.
//

#include "procedure_call.h"
namespace gfx::compiler {

procedure_call::procedure_call(element *parent, compiler::type *procedure_type, element *expr)
	: element(parent, element_type_t::proc_call), expression_(expr), procedure_type_(procedure_type)
{

}
element *procedure_call::expression()
{
	return expression_;
}
compiler::type *procedure_call::procedure_type()
{
	return procedure_type_;
}
}