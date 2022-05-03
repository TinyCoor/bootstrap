//
// Created by 12132 on 2022/5/3.
//

#include "line_comment.h"
namespace gfx::compiler {
line_comment::line_comment(element* parent, const std::string& value)
	: element(parent),value_(value)
{

}

line_comment::line_comment(element* parent,std::string &&value)
	: element(parent),value_(std::move(value))
{}
}