//
// Created by 12132 on 2022/4/23.
//

#include "type.h"
#include "field.h"
namespace gfx::compiler {
type::type() {
}

type::~type() {
	for (auto field : fields_) {
		delete field.second;
	}
	fields_.clear();
}

void type::add_field(const std::string& name, compiler::type* type, compiler::initializer* initializer)
{
	auto field = new compiler::field(name, type, initializer);
	field->type(type);
	fields_.insert(std::make_pair(name, field));
}

bool type::remove_field(const std::string& name) {
	return fields_.erase(name) > 0;
}

compiler::field* type::find_field(const std::string& name) {
	auto it = fields_.find(name);
	if (it != fields_.end()) {
		return it->second;
	}
	return nullptr;
}

}