//
// Created by 12132 on 2022/5/29.
//

#include "unknown_type.h"
namespace gfx::compiler {
unknown_type::unknown_type(compiler::module* module, block *parent_scope, compiler::symbol_element *symbol)
    : type(module, parent_scope, element_type_t::unknown_type, symbol)
{

}
bool unknown_type::is_array() const
{
    return is_array_;
}

void unknown_type::is_array(bool value)
{
    is_array_ = value;
}

size_t unknown_type::array_size() const
{
    return array_size_;
}

void unknown_type::array_size(size_t value)
{
    array_size_ = value;
}

bool unknown_type::is_pointer() const
{
    return is_pointer_;
}

void unknown_type::is_pointer(bool value)
{
    is_pointer_ = value;
}

bool unknown_type::on_initialize(compiler::session& session)
{
    return true;
}


}
