//
// Created by 12132 on 2022/11/4.
//

#include "type_of_intrinsic.h"
#include "compiler/session.h"
#include "compiler/elements/types/pointer_type.h"
#include "compiler/elements/integer_literal.h"
#include "compiler/elements/argument_list.h"
namespace gfx::compiler {
type_of_intrinsic::type_of_intrinsic(compiler::module *module, compiler::block *parent_scope,
    compiler::argument_list *args)
        : intrinsic(module, parent_scope, args)
{

}
std::string_view type_of_intrinsic::name() const
{
    return "type_of";
}

bool type_of_intrinsic::on_is_constant() const
{
    return true;
}

bool type_of_intrinsic::on_fold(session &session, fold_result_t &result)
{
    auto args = arguments()->elements();
    if (args.empty() || args.size() > 1) {
        session.error(this, "P091", "type_of expects a single argument.", location());
        return false;
    }
    type_inference_result_t type_result;
    args[0]->infer_type(session, type_result);
    auto arg_type = type_result.type;
    result.element = session.builder().make_integer(parent_scope(), 0xdeadbeef);
    return true;
}

bool type_of_intrinsic::on_infer_type(const session &session, type_inference_result_t &result)
{
    auto& scope_manager = session.scope_manager();
    auto type_info_type = scope_manager.find_type(qualified_symbol_t {
        .name = "type"
    });
    auto ptr_type = scope_manager.find_pointer_type(type_info_type, parent_scope());
    if (ptr_type == nullptr) {
        ptr_type = const_cast<compiler::session&>(session).builder()
            .make_pointer_type(parent_scope(), type_info_type);
    }
    return ptr_type;
}
}