//
// Created by 12132 on 2022/11/4.
//

#include "assembly_label.h"
#include "compiler/session.h"
#include <fmt/format.h>
namespace gfx::compiler {

assembly_label::assembly_label(compiler::module *module, block *parent_scope, const std::string &name)
    : element(module, parent_scope, element_type_t::assembly_label), name_(name)
{

}
std::string assembly_label::name() const
{
    return name_;
}

bool assembly_label::on_is_constant() const
{
    return true;
}

bool assembly_label::on_emit(session &session)
{
    auto& assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    auto target_reg = assembler.current_target_register();

    instruction_block->blank_line();
    instruction_block->comment(fmt::format("assembly_label address: {}", name_), 4);
    auto label_ref = assembler.make_label_ref(name_);
    instruction_block->move_label_to_reg(*target_reg, label_ref);

    return true;
}

bool assembly_label::on_infer_type(const session &session, type_inference_result_t &result)
{
    result.type = session.scope_manager().find_type(qualified_symbol_t {
        .name = "u64"
    });
    return result.type != nullptr;
}
}