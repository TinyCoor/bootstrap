//
// Created by 12132 on 2022/4/23.
//

#include "element.h"
namespace gfx::compiler {
element::element(element* parent, element_type_t type)
	:  id_(id_pool::instance()->allocate()), parent_(parent),element_type_(type)
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

}