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

bool compiler::unary_operator::on_emit(gfx::result &r, gfx::assembler &assembler, const emit_context_t& context)
{
    auto instruction_block = assembler.current_block();
    auto target_reg = instruction_block->current_target_register();
    auto rhs_reg = instruction_block->allocate_ireg();
    instruction_block->push_target_register(rhs_reg);
    rhs_->emit(r, assembler, context);
    instruction_block->pop_target_register();

    switch (operator_type()) {
        case operator_type_t::negate: {
            instruction_block->neg<uint64_t>(target_reg->reg.i, rhs_reg);
            break;
        }
        case operator_type_t::binary_not: {
            instruction_block->not_reg<uint64_t>(target_reg->reg.i, rhs_reg);
            break;
        }
        case operator_type_t::logical_not: {
            break;
        }
        default:
            break;
    }

    instruction_block->free_ireg(rhs_reg);

    return true;
}
}