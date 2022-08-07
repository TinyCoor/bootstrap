//
// Created by 12132 on 2022/4/23.
//
#include "common/id_pool.h"
#include "element.h"
#include "types/type.h"
#include "vm/assembler.h"
#include "fmt/format.h"

namespace gfx::compiler {
element::element(block* parent_scope, element_type_t type, element* parent_element)
	:  id_(id_pool::instance()->allocate()), parent_scope_(parent_scope),
        parent_element_(parent_element), element_type_(type)
{
}

element::~element() = default;

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

block *element::parent_scope()
{
    return parent_scope_;
}

attribute_map_t &element::attributes()
{
	return attributes_;
}

element_type_t element::element_type() const
{
	return element_type_;
}

compiler::type *element::infer_type(const compiler::program *program)
{
	switch (element_type_) {
		case element_type_t::any_type:
		case element_type_t::proc_type:
		case element_type_t::bool_type:
		case element_type_t::alias_type:
		case element_type_t::array_type:
		case element_type_t::string_type:
		case element_type_t::numeric_type:
		case element_type_t::composite_type:
		case element_type_t::namespace_type:
			return dynamic_cast<compiler::type*>(this);
		default:
			return on_infer_type(program);
	}
}

compiler::type *element::on_infer_type(const compiler::program *program)
{
	return nullptr;
}

bool element::as_bool(bool &value)
{
    return on_as_bool(value);
}

bool element::as_float(double &value)
{
    return on_as_float(value);
}

bool element::as_integer(uint64_t &value)
{
    return on_as_integer(value);
}

bool element::as_string(std::string &value)
{
    return on_as_string(value);
}

bool element::on_as_string(std::string &value) const
{
    return false;
}

bool element::on_as_bool(bool &value) const
{
    return false;
}

bool element::on_as_float(double &value) const
{
    return false;
}

bool element::on_as_integer(uint64_t &value) const
{
    return false;
}

bool element::is_constant() const
{
    return on_is_constant();
}

bool element::on_is_constant() const
{
    return false;
}

bool element::emit(result &r,  emit_context_t& context)
{
    return on_emit(r, context);
}

bool compiler::element::on_emit(gfx::result &r, emit_context_t& context)
{
    return true;
}

std::string element::label_name() const
{
    return fmt::format("{}_{}", element_type_name(element_type_), id_);
}

element *element::parent_element()
{
    return parent_element_;
}

void element::parent_element(element *parent)
{
    parent_element_ = parent;
}

bool element::is_parent_element(element_type_t type)
{
    if (parent_element_ == nullptr) {
        return false;
    }
    return parent_element_->element_type() == type;
}
}

