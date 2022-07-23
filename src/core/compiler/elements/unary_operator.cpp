//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
#include "program.h"
namespace gfx::compiler {
unary_operator::unary_operator(element* parent, operator_type_t type,element* rhs)
	: operator_base(parent, element_type_t::unary_operator, type), rhs_(rhs)
{
}

element* unary_operator::rhs()
{
	return rhs_;
}
/// TODO more
compiler::type *unary_operator::on_infer_type(const compiler::program *program)
{
	switch (operator_type()) {
		case operator_type_t::negate:
		case operator_type_t::binary_not: {
			return program->find_type("u64");
		}
		case operator_type_t::logical_not: {
			return program->find_type("bool");
		}
		default:
			return nullptr;
	}
}

bool unary_operator::on_is_constant() const
{
    return rhs_ != nullptr && rhs_->is_constant();
}

bool compiler::unary_operator::on_emit(gfx::result &r, gfx::assembler &assembler)
{
    auto instruction_block = assembler.current_block();
    switch (operator_type()) {
        case operator_type_t::negate: {
            // XXX: how to best handle the rhs here?
            auto src_reg = instruction_block->allocate_ireg();
            instruction_block->move_u64_to_ireg(src_reg, 0xc0fefe);

            auto dest_reg = instruction_block->allocate_ireg();
            instruction_block->neg_u64(dest_reg, src_reg);
            instruction_block->free_ireg(dest_reg);
            instruction_block->free_ireg(src_reg);
            break;
        }
        case operator_type_t::binary_not:
            break;
        case operator_type_t::logical_not:
            break;
        default:
            break;
    }
    return true;
}
}