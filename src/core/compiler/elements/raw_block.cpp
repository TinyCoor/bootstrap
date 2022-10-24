//
// Created by 12132 on 2022/10/24.
//

#include "raw_block.h"
namespace gfx::compiler {

raw_block::raw_block(compiler::module *module, block *parent_scope, const std::string &value)
    : element(module, parent_scope, element_type_t::raw_block), value_(value)
{

}
std::string raw_block::value() const
{
    return value_;
}
}