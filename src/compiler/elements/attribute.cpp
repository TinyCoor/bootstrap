//
// Created by 12132 on 2022/4/23.
//

#include "attribute.h"
namespace gfx::compiler{
attribute::attribute(const std::string &name, expression *rhs)
	: name_(name), rhs_(rhs)
{}

attribute::~attribute() noexcept {

}
}