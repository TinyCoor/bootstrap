//
// Created by 12132 on 2022/10/29.
//

#include "size_of_intrinsic.h"
#include "compiler/session.h"
#include "compiler/elements/integer_literal.h"
#include "compiler/elements/argument_list.h"
namespace gfx::compiler {

size_of_intrinsic::size_of_intrinsic(compiler::module *module, compiler::block *parent_scope,
                                     compiler::argument_list *args)
    : intrinsic(module, parent_scope, args)
{

}
bool size_of_intrinsic::on_fold(session &session, fold_result_t& result)
{
    auto args = arguments()->elements();
    if (args.empty() || args.size() > 1) {
        session.error(this, "P091", "size_of expects a single argument.", location());
        return false;
    }
    type_inference_result_t type_result;
    args[0]->infer_type(session, type_result);
    auto arg_type = type_result.type;
    result.element = session.builder().make_integer(parent_scope(), arg_type->size_in_bytes());
    return true;
}

bool size_of_intrinsic::on_infer_type(const session &session, type_inference_result_t &result)
{
    result.type = session.scope_manager().find_type(qualified_symbol_t {
        .name = "u32"
    });
    return result.type != nullptr;
}

bool size_of_intrinsic::on_is_constant() const
{
    return true;
}
std::string_view size_of_intrinsic::name() const
{
    return "size_of";
}

}