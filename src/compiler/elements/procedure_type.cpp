//
// Created by 12132 on 2022/5/2.
//

#include "procedure_type.h"
namespace gfx::compiler {
procedure_type::procedure_type(element* parent, const std::string name)
	: type(parent,name),  returns_(this), parameters_(this)
{

}

}