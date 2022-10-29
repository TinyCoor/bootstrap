//
// Created by 12132 on 2022/10/29.
//

#include "size_of_intrinsic.h"
#include "compiler/session.h"
namespace gfx::compiler {

size_of_intrinsic::size_of_intrinsic(compiler::module *module, compiler::block *parent_scope,
                                     compiler::argument_list *args)
    : intrinsic(module, parent_scope, args)
{

}
compiler::element *size_of_intrinsic::on_fold(session &session)
{
    return element::on_fold(session);
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

}