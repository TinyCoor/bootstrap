//
// Created by 12132 on 2022/5/3.
//

#include "composite_type.h"
#include "identifier.h"
namespace gfx::compiler {
composite_type::composite_type(element* parent, composite_types_t type, const std::string& name,
    element_type_t element_type)
	: compiler::type(parent, element_type, name), type_(type)
{

}

field_map_t& composite_type::fields()
{
	return fields_;
}

type_map_t& composite_type::type_parameters()
{
	return type_parameters_;
}

composite_types_t composite_type::type() const
{
	return type_;
}

bool composite_type::on_initialize(result &r, compiler::program* program)
{
    size_t size = 0;
    for (auto &filed : fields_.as_list()) {
        size += filed->identifier()->type()->size_in_bytes();
    }
    size_in_bytes(size);
	return true;
}
}