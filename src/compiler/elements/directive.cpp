//
// Created by 12132 on 2022/4/23.
//

#include "directive.h"
namespace gfx::compiler {
directive::directive(block* parent, const std::string& name)
	: block(parent),name_(name)
{
}

directive::~directive()
{
}

std::string directive::name() const
{
	return name_;
}

}