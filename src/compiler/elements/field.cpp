//
// Created by 12132 on 2022/4/23.
//

#include "field.h"
namespace gfx::compiler {

field::field(element *parent, compiler::identifier *identifier)
	: element(parent,element_type_t::field), identifier_(identifier)
{

}

compiler::identifier *field::identifier()
{
	return identifier_;
}

}