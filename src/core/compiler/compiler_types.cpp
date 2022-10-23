//
// Created by 12132 on 2022/10/23.
//
#include "compiler_types.h"
#include "elements/types/type.h"
#include "elements/identifier.h"
#include "elements/symbol_element.h"
namespace gfx::compiler {

std::string type_inference_result_t::name() const
{
    if (identifier != nullptr && identifier->type_alias()) {
        return identifier->symbol()->name();
    }
    if (type != nullptr) {
        return type->symbol()->name();
    }
    return "unknown";
}
}