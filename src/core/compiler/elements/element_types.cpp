//
// Created by 12132 on 2022/5/3.
//

#include "element_types.h"
#include "attribute.h"
#include "field.h"
#include "identifier.h"
#include "symbol_element.h"
#include "types/type.h"

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
	fields_.insert(std::make_pair(value->id(), value));
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

bool field_map_t::remove(id_t id)
{
	return fields_.erase(id) > 0;
}

compiler::field* field_map_t:: find(id_t id)
{
	auto it = fields_.find(id);
	if (it != fields_.end()) {
		return it->second;
	}
	return nullptr;
}


//////////////////////////////////////////////////////

void identifier_map_t::add(identifier* value)
{
	identifiers_.insert(std::make_pair(value->symbol()->name(), value));
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
	types_.insert(std::make_pair(type->symbol()->name(), type));
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

type_list_t type_map_t::as_list() const
{
    type_list_t list {};
    for (const auto& it : types_) {
        list.push_back(it.second);
    }
    return list;
}
///////////////////////////////////////////////////////////////////////////

std::string make_fully_qualified_name(const symbol_element* symbol)
{
    std::stringstream stream {};
    auto count = 0;
    for (const auto& name : symbol->namespaces()) {
        if (count > 0) {
            stream << "::";
        }
        stream << name;
        count++;
    }
    if (count > 0) {
        stream << "::";
    }

    stream << symbol->name();
    return stream.str();
}

std::string make_fully_qualified_name(const qualified_symbol_t& symbol)
{
    std::stringstream stream {};
    auto count = 0;
    for (const auto& name : symbol.namespaces) {
        if (count > 0) {
            stream << "::";
        }
        stream << name;
        count++;
    }
    if (count > 0) {
        stream << "::";
    }

    stream << symbol.name;
    return stream.str();
}

element_register_t::~element_register_t()
{
    if (assembler == nullptr) {
        return;
    }

    if (clean_up) {
        if (var != nullptr) {
            var->make_dormat(assembler);
        } else {
            if (integer) {
                assembler->free_reg(reg.i);
            }else {
                assembler->free_reg(reg.f);
            }
        }
    }
}
op_sizes element_register_t::size() const
{
    if (var != nullptr) {
        return op_size_for_byte_size(var->type->size_in_bytes());
    }
    return op_sizes::qword;
}
}