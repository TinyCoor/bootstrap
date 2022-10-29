//
// Created by 12132 on 2022/10/29.
//

#include "alloc_intrinsic.h"
#include "compiler/session.h"
#include "compiler/elements/argument_list.h"
namespace gfx::compiler {
alloc_intrinsic::alloc_intrinsic(compiler::module *module, compiler::block *parent_scope, compiler::argument_list *args)
    : intrinsic(module, parent_scope, args)
{

}
bool alloc_intrinsic::on_emit(session &session)
{
    auto& assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    auto target_reg = assembler.current_target_register();

    auto args = arguments()->elements();
    if (args.empty() || args.size() > 1) {
        session.error(this, "P091", "alloc expects a single integer argument.", location());
        return false;
    }
    // XXX: needs error handling
    auto arg = args[0];
    type_inference_result_t result;
    arg->infer_type(session, result);
    if (result.type == nullptr ||  result.type->number_class() != type_number_class_t::integer) {
        session.error(this, "P091", "free expects a single integer argument.", location());
        return false;
    }
    auto arg_reg = register_for(session, arg);
    if (arg_reg.var != nullptr) {
        arg_reg.clean_up = true;
    }

    assembler.push_target_register(arg_reg.reg);
    arg->emit(session);
    assembler.pop_target_register();

    instruction_block->alloc(op_sizes::byte, *target_reg, arg_reg.reg);

    return true;
}

bool alloc_intrinsic::on_infer_type(const session &session, type_inference_result_t &result)
{
    result.type = session.scope_manager().find_type(qualified_symbol_t {
        .name = "u64"
    });
    return result.type != nullptr;
}
}