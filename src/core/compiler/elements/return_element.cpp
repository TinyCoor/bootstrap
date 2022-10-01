//
// Created by 12132 on 2022/5/14.
//

#include "return_element.h"
namespace gfx::compiler {

return_element::return_element(block *parent)
	: element(parent, element_type_t::return_e)
{

}

element_list_t &return_element::expressions()
{
	return expressions_;
}

bool compiler::return_element::on_emit(gfx::result &r, emit_context_t &context )
{
    auto assembler = context.assembler;
    auto instruction_block = assembler->current_block();
    if (!expressions_.empty()) {
        registers_t target_reg;
        if (!assembler->allocate_reg(target_reg)) {
        }
        assembler->push_target_register(op_sizes::qword, target_reg);
        expressions_.front()->emit(r, context);
        instruction_block->store_from_ireg(op_sizes::qword, registers_t::fp, target_reg, 8);
        assembler->pop_target_register();
        assembler->free_reg(target_reg);
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