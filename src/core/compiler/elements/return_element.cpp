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
    auto instruction_block = context.assembler->current_block();
    if (!expressions_.empty()) {
        i_registers_t target_reg;
        if (!instruction_block->allocate_reg(target_reg)) {
        }
        instruction_block->push_target_register(target_reg);
        expressions_.front()->emit(r, context);
        instruction_block->store_from_ireg<uint64_t>(i_registers_t::fp, target_reg, 8);
        instruction_block->pop_target_register();
        instruction_block->free_reg(target_reg);

    }
    instruction_block->rts();
    return true;
}
}