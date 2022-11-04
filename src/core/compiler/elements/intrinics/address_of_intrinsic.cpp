//
// Created by 12132 on 2022/11/4.
//

#include "address_of_intrinsic.h"
#include "compiler/session.h"
#include "compiler/elements/symbol_element.h"
#include "compiler/elements/assembly_label.h"
#include "compiler/elements/identifier.h"
#include "compiler/elements/identifier_reference.h"
#include "compiler/elements/integer_literal.h"
#include "compiler/elements/argument_list.h"
namespace gfx::compiler {
address_of_intrinsic::address_of_intrinsic(compiler::module *module,
    compiler::block *parent_scope, compiler::argument_list *args)
    : intrinsic(module, parent_scope, args)
{

}

std::string_view address_of_intrinsic::name() const
{
    return "address_of";
}

bool address_of_intrinsic::on_is_constant() const
{
    return true;
}

bool address_of_intrinsic::on_fold(session &session, fold_result_t &result)
{
    auto args = arguments()->elements();
    if (args.empty() || args.size() > 1) {
        session.error(this, "P091", "address_of expects a single argument.", location());
        return false;
    }

    auto arg = args[0];
    if (arg==nullptr || arg->element_type() != element_type_t::identifier_reference) {
        session.error(this, "P091", "address_of expects an identifier reference parameter.", location());
        return false;
    }
    auto ref = dynamic_cast<compiler::identifier_reference*>(arg);
    result.element = session.builder().make_assembly_label(parent_scope(),
        ref->identifier()->symbol()->name());
    return true;
}
bool address_of_intrinsic::on_infer_type(const session &session, type_inference_result_t &result)
{
    result.type = session.scope_manager().find_type(qualified_symbol_t{
        .name = "u64"
    });
    return result.type!=nullptr;
}
}