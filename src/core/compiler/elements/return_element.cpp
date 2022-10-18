//
// Created by 12132 on 2022/5/14.
//

#include "return_element.h"
#include "core/compiler/session.h"
namespace gfx::compiler {

return_element::return_element(compiler::module* module,block *parent)
	: element(module, parent, element_type_t::return_e)
{

}

element_list_t &return_element::expressions()
{
	return expressions_;
}

bool compiler::return_element::on_emit(compiler::session &session)
{
    auto &assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    if (!expressions_.empty()) {
        register_t target_reg;
        if (!assembler.allocate_reg(target_reg)) {
        }
        assembler.push_target_register(target_reg);
        expressions_.front()->emit(session);
        instruction_block->store_from_reg(register_t::fp(), target_reg, 8);
        assembler.pop_target_register();
        assembler.free_reg(target_reg);
    }
    instruction_block->rts();
    return true;
}

void return_element::on_owned_elements(element_list_t &list)
{
    for (auto element : expressions_) {
        list.emplace_back(element);
    }
}
}