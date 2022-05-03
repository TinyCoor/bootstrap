//
// Created by 12132 on 2022/4/23.
//

#include "element.h"
namespace gfx::compiler {
element::element(element* parent)
	:  id_(id_pool::instance()->allocate()), parent_(parent), attributes_(this)
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

}