//
// Created by 12132 on 2022/5/2.
//

#include "procedure_type.h"
namespace gfx::compiler {
procedure_type::procedure_type(element* parent, const std::string name)
	: type(parent, element_type_t::proc_type,name)
{

}

field_map_t& procedure_type::returns() {
	return returns_;
}

field_map_t& procedure_type::parameters() {
	return parameters_;
}

type_map_t& procedure_type::type_parameters() {
	return type_parameters_;
}

}