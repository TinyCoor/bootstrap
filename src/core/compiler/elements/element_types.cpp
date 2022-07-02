//
// Created by 12132 on 2022/5/3.
//

#include "element_types.h"
#include "attribute.h"
#include "field.h"
#include "identifier.h"
#include "type.h"

namespace gfx::compiler {

size_t attribute_map_t::size() const
{
	return attrs_.size();
}

bool attribute_map_t::remove(const std::string &name)
{
	return attrs_.erase(name) > 0;
}

attribute_list_t attribute_map_t::as_list() const
{
	attribute_list_t list {};
	for (const auto& it : attrs_) {
		list.push_back(it.second);
	}
	return list;
}

compiler::attribute *attribute_map_t::find(const std::string &name)
{
	auto it = attrs_.find(name);
	if (it != attrs_.end()) {
		return it->second;
	}
	return nullptr;
}
void attribute_map_t::add(attribute* value)
{
	attrs_.insert(std::make_pair(value->name(), value));
}

//////////////////////////////////////////////////////////////////////////////////////////
void field_map_t:: add(field* value)
{
	fields_.insert(std::make_pair(value->identifier()->name(), value));
}

field_list_t field_map_t::as_list()
{
	field_list_t list {};
	for (const auto& it : fields_) {
		list.push_back(it.second);
	}
	return list;
}

size_t field_map_t:: size() const
{
	return fields_.size();
}

bool field_map_t:: remove(const std::string& name)
{
	return fields_.erase(name) > 0;
}

compiler::field* field_map_t:: find(const std::string& name)
{
	auto it = fields_.find(name);
	if (it != fields_.end()) {
		return it->second;
	}
	return nullptr;
}


//////////////////////////////////////////////////////

void identifier_map_t::add(identifier* value)
{
	identifiers_.insert(std::make_pair(value->name(), value));
}

size_t identifier_map_t::size() const
{
	return identifiers_.size();
}

identifier_list_t identifier_map_t::as_list() const {
	identifier_list_t list {};
	for (const auto& it : identifiers_) {
		list.push_back(it.second);
	}
	return list;
}

identifier_list_t identifier_map_t::globals(bool initialized)
{
	identifier_list_t list {};
	for (const auto& it : identifiers_) {
		if (it.second->constant()) {
			continue;
		}

		auto init = it.second->initializer();
		if (!initialized) {
			if (init == nullptr) {
				list.push_back(it.second);
			}
		}
		else {
			if (init != nullptr) {
				if (init->expression()->element_type() == element_type_t::namespace_e
					||  init->expression()->element_type() == element_type_t::proc_type) {
					continue;
				}
				list.push_back(it.second);
			}
		}
	}
	return list;
}

identifier_list_t identifier_map_t::constants(bool initialized)
{
	identifier_list_t list {};
	for (const auto& it : identifiers_) {
		if (!it.second->constant()) {
			continue;
		}
		auto init = it.second->initializer();
		if (!initialized) {
			if (init == nullptr) {
				list.push_back(it.second);
			}
		} else {
			if (init != nullptr) {
				list.push_back(it.second);
			}
		}
	}
	return list;
}

bool identifier_map_t::remove(const std::string& name)
{
	return identifiers_.erase(name) > 0;
}

identifier* identifier_map_t::find(const std::string& name)
{
	auto it = identifiers_.find(name);
	if (it != identifiers_.end()) {
		return it->second;
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////
void type_map_t::add(compiler::type* type)
{
	types_.insert(std::make_pair(type->name(), type));
}

size_t type_map_t::size() const
{
	return types_.size();
}

bool type_map_t::remove(const std::string& name)
{
	return types_.erase(name) > 0;
}

compiler::type* type_map_t::find(const std::string& name)
{
	auto it = types_.find(name);
	if (it != types_.end()) {
		return it->second;
	}
	return nullptr;
}


}