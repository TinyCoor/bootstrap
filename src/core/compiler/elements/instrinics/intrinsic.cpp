//
// Created by 12132 on 2022/10/22.
//

#include "intrinsic.h"
#include "compiler/elements/argument_list.h"
#include "compiler/session.h"
namespace gfx::compiler {

intrinsic::intrinsic(compiler::module *module, compiler::block *parent_scope,
    compiler::argument_list *args)
     : element(module, parent_scope, element_type_t::intrinsic),
         arguments_(args)
{

}
compiler::argument_list *intrinsic::arguments()
{
    return arguments_;
}

void intrinsic::on_owned_elements(element_list_t &list)
{
    if (arguments_) {
        list.push_back(arguments_);
    }
}
intrinsic *intrinsic::intrinsic_for_call(session &session, compiler::block *parent_scope,
    compiler::argument_list *args, const qualified_symbol_t& symbol)
{
    auto& builder = session.builder();

    intrinsic* element = nullptr;
    if (symbol.name == "size_of") {
        element = builder.make_size_of_intrinsic(parent_scope, args);
    } else if (symbol.name == "alloc") {
        element = builder.make_alloc_intrinsic(parent_scope, args);
    } else if (symbol.name == "free") {
        element = builder.make_free_intrinsic(parent_scope, args);
    }

    if (element != nullptr) {
        element->location(symbol.location);
    }
    return element;
}
std::string_view intrinsic::name() const
{
    return "intrinsic";
}

}