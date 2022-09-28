//
// Created by 12132 on 2022/4/23.
//
#include "common/id_pool.h"
#include "element.h"
#include "types/type.h"
#include "vm/assembler.h"
#include "fmt/format.h"
#include "program.h"
namespace gfx::compiler {
element::element(block *parent_scope, element_type_t type, element *parent_element)
    : id_(id_pool::instance()->allocate()), parent_scope_(parent_scope),
      parent_element_(parent_element), element_type_(type)
{
}

element::~element() = default;

id_t element::id() const
{
    return id_;
}

element *element::fold(result &result, compiler::program *program)
{
    auto no_fold_attribute = find_attribute("no_fold");
    if (no_fold_attribute!=nullptr) {
        return nullptr;
    }
    return on_fold(result, program);
}

element *element::on_fold(result &result, compiler::program *program)
{
    return nullptr;
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
        case element_type_t::module_type:
        case element_type_t::string_type:
        case element_type_t::numeric_type:
        case element_type_t::pointer_type:
        case element_type_t::composite_type:
        case element_type_t::namespace_type: return dynamic_cast<compiler::type *>(this);
        default: return on_infer_type(program);
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

bool element::emit(result &r, emit_context_t &context)
{
    return on_emit(r, context);
}

bool compiler::element::on_emit(gfx::result &r, emit_context_t &context)
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
    if (parent_element_==nullptr) {
        return false;
    }
    return parent_element_->element_type()==type;
}

attribute *element::find_attribute(const std::string &name)
{
    auto current_element = this;
    while (current_element!=nullptr) {
        auto attr = current_element->attributes_.find(name);
        if (attr!=nullptr) {
            return attr;
        }
        current_element = current_element->parent_element();
    }
    return nullptr;
}

void element::owned_elements(element_list_t &list)
{
    on_owned_elements(list);
}

void element::on_owned_elements(element_list_t &list)
{

}
const source_location &element::location() const
{
    return location_;
}

void element::location(const source_location &location)
{
    location_ = location;
}

bool element::is_type() const
{
    switch (element_type_) {
        case element_type_t::any_type:
        case element_type_t::proc_type:
        case element_type_t::bool_type:
        case element_type_t::type_info:
        case element_type_t::alias_type:
        case element_type_t::array_type:
        case element_type_t::tuple_type:
        case element_type_t::string_type:
        case element_type_t::module_type:
        case element_type_t::numeric_type:
        case element_type_t::pointer_type:
        case element_type_t::composite_type:
        case element_type_t::namespace_type:return true;
        default:return false;
    }
}

element_register_t element::register_for(result &r, emit_context_t &context, element *e)
{
    element_register_t result {.assembler = context.assembler};

    auto var = context.variable_for_element(e);
    if (var != nullptr) {
        var->make_live(context.assembler);
        result.var = var;
        result.var->read(context.assembler, context.assembler->current_block());
        result.valid = true;
        result.integer = var->value_reg.integer;
        if (var->address_reg.allocated && var->type->access_model() == type_access_model_t::pointer) {
            result.reg.i = var->address_reg.value.i;
        } else {
            if (result.integer) {
                result.reg.i = result.var->value_reg.value.i;
            } else {
                result.reg.f = result.var->value_reg.value.f;
            }
        }
    } else {
        i_registers_t reg;
        if (!context.assembler->allocate_reg(reg)) {
            context.program->error(r, e, "P052", "assembler registers exhausted.", e->location());
        } else {
            result.reg.i = reg;
            result.valid = true;
            result.integer = true;
            result.clean_up = true;
        }
    }

    return result;
}
}
