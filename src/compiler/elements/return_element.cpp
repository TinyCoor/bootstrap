//
// Created by 12132 on 2022/5/14.
//

#include "return_element.h"
namespace gfx::compiler {

return_element::return_element(element *parent)
	: element(parent, element_type_t::return_e)
{

}

element_list_t &return_element::expressions()
{
	return expressions_;
}
}