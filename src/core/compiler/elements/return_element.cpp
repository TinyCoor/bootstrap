//
// Created by 12132 on 2022/5/14.
//

#include "return_element.h"
namespace gfx::compiler {

return_element::return_element(element *parent)
	: element(parent, element_type_t::return_e)
{

}

element_list_t &return_element::expressions()
{
	return expressions_;
}

bool compiler::return_element::on_emit(gfx::result &r, gfx::assembler &assembler)
{
    auto instruction_block = assembler.current_block();

    for (auto expr : expressions_) {
        // XXX: why is expr null?
        if (expr != nullptr) {
            expr->emit(r, assembler);
        }
    }

    instruction_block->rts();

    return true;
}
}