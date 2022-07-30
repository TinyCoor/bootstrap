//
// Created by 12132 on 2022/4/23.
//
#include "common/id_pool.h"
#include "element.h"
#include "type.h"
#include "vm/assembler.h"
#include "fmt/format.h"

namespace gfx::compiler {
element::element(element* parent, element_type_t type)
	:  id_(id_pool::instance()->allocate()), parent_(parent), element_type_(type)
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

element *element::parent()
{
	return parent_;
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

bool element::emit(result &r, assembler &assembler, emit_context_t& context)
{
    return on_emit(r, assembler, context);
}

bool compiler::element::on_emit(gfx::result &r, gfx::assembler &assembler, emit_context_t& context)
{
    return true;
}

std::string element::label_name() const
{
    return fmt::format("{}_{}", element_type_name(element_type_),
        id_);
}
}

