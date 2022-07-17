//
// Created by 12132 on 2022/7/17.
//

#include "struct_type.h"
namespace gfx::compiler {
struct_type::struct_type(element *parent, const std::string &name)
    : composite_type(parent, composite_types_t::struct_type, name, element_type_t::struct_type)
{

}
}
