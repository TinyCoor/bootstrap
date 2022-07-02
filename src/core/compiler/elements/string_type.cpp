//
// Created by 12132 on 2022/5/2.
//

#include "string_type.h"
namespace gfx::compiler {
string_type::string_type(element* parent)
	: type(parent, element_type_t::string_type, "string")
{

}
}