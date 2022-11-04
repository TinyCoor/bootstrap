//
// Created by 12132 on 2022/9/17.
//

#include "pointer_type.h"
#include "fmt/format.h"
#include "core/compiler/session.h"
#include "../symbol_element.h"
namespace gfx::compiler {
std::string pointer_type::name_for_pointer(compiler::type* base_type)
{
    return fmt::format("__ptr_{}__", base_type->symbol()->name());
}

pointer_type::pointer_type(compiler::module* module, compiler::block* parent_scope, compiler::type* base_type)
    : compiler::type(module, parent_scope, element_type_t::pointer_type, nullptr), base_type_(base_type)
{
}

bool pointer_type::on_initialize(compiler::session& session)
{
    auto type_symbol = session.builder().make_symbol(parent_scope(), name_for_pointer(base_type_));
    symbol(type_symbol);
    type_symbol->parent_element(this);
    size_in_bytes(sizeof(uint64_t));
    return true;
}

compiler::type* pointer_type::base_type() const
{
    return base_type_;
}

type_access_model_t pointer_type::on_access_model() const
{
    return type_access_model_t::pointer;
}
bool pointer_type::on_type_check(compiler::type *other)
{
    if (other == nullptr || other->element_type() != element_type_t::pointer_type) {
        return false;
    }
    auto other_pointer_type = dynamic_cast<compiler::pointer_type*>(other);
    return base_type()->id() == other_pointer_type->base_type()->id();

}
}