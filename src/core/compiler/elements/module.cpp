//
// Created by 12132 on 2022/8/31.
//

#include "module.h"
#include "block.h"
namespace gfx::compiler {
module::module(compiler::block *parent_scope, compiler::block *scope)
    : element(parent_scope, element_type_t::module), scope_(scope)
{

}

compiler::block *module::scope()
{
    return scope_;
}

void module::on_owned_elements(element_list_t &list)
{
    if (scope_ !=nullptr) {
        list.emplace_back(scope_);
    }
}

bool module::on_emit(result &r, emit_context_t &context)
{
    if (scope_ == nullptr) {
        return true;
    }
    return scope_->emit(r, context);
}

gfx::source_file *module::source_file() const
{
    return source_file_;
}

void module::source_file(gfx::source_file *value)
{
    source_file_ = value;
}

bool module::is_root() const
{
    return is_root_;
}

void module::is_root(bool value)
{
    is_root_ = value;
}

}