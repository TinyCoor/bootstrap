//
// Created by 12132 on 2022/5/2.
//

#include "procedure_type.h"
namespace gfx::compiler {
procedure_type::procedure_type(element* parent, compiler::block* scope, const std::string name)
	: type(parent, element_type_t::proc_type, name), scope_(scope)
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

procedure_instance_list_t& procedure_type::instances() {
	return instances_;
}

compiler::block *procedure_type::scope()
{
	return scope_;
}

}