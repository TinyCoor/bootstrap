//
// Created by 12132 on 2022/5/3.
//

#include "comment.h"
namespace gfx::compiler {
comment::comment(element *parent, comment_type_t type,const std::string &value)
	: element(parent,element_type_t::comment),value_(value), type_(type)
{

}

std::string comment::value() const {
	return value_;
}

comment_type_t comment::type() const
{
	return type_;
}
}