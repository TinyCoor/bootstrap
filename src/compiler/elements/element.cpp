//
// Created by 12132 on 2022/4/23.
//

#include "element.h"
namespace gfx::compiler {
element::element()
	: id_(id_pool::instance()->allocate())
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

bool element::remove_attribute(const std::string& name)
{
	return attributes_.erase(name) > 0;
}

bool element::remove_directive(const std::string& name)
{
	return directives_.erase(name) > 0;
}

attribute* element::find_attribute(const std::string& name)
{
	auto it = attributes_.find(name);
	if (it != attributes_.end()) {
		return it->second;
	}
	return nullptr;
}

directive* element::find_directive(const std::string& name)
{
	auto it = directives_.find(name);
	if (it != directives_.end()) {
		return it->second;
	}
	return nullptr;
}

}