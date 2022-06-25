//
// Created by 12132 on 2022/4/23.
//
#include <common/id_pool.h>
#include "element.h"
#include "type.h"

namespace gfx::compiler {
element::element(element* parent, element_type_t type)
	:  id_(id_pool::instance()->allocate()), parent_(parent), element_type_(type)
{
}

element::~element()
{
}

id_t element::id() const
{
	return id_;
}

bool element::fold(result& result)
{
	return on_fold(result);
}

bool element::on_fold(result& result)
{
	return true;
}

element *element::parent()
{
	return parent_;
}

attribute_map_t &element::attributes()
{
	return attributes_;
}

element_type_t element::element_type() const
{
	return element_type_;
}

compiler::type *element::infer_type(const compiler::program *program)
{
	switch (element_type_) {
		case element_type_t::any_type:
		case element_type_t::proc_type:
		case element_type_t::bool_type:
		case element_type_t::alias_type:
		case element_type_t::array_type:
		case element_type_t::string_type:
		case element_type_t::numeric_type:
		case element_type_t::composite_type:
		case element_type_t::namespace_type:
			return dynamic_cast<compiler::type*>(this);
		default:
			return on_infer_type(program);
	}
}

compiler::type *element::on_infer_type(const compiler::program *program)
{
	return nullptr;
}

}