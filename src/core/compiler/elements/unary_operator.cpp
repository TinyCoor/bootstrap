//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
#include "program.h"
namespace gfx::compiler {
unary_operator::unary_operator(block* parent, operator_type_t type,element* rhs)
	: operator_base(parent, element_type_t::unary_operator, type), rhs_(rhs)
{
}

element* unary_operator::rhs()
{
	return rhs_;
}

compiler::type *unary_operator::on_infer_type(const compiler::program *program)
{
	switch (operator_type()) {
		case operator_type_t::negate:
		case operator_type_t::binary_not: {
			return program->find_type(qualified_symbol_t{.name= "u64"});
		}
		case operator_type_t::logical_not: {
			return program->find_type(qualified_symbol_t{.name = "bool"});
		}
		default:
			return nullptr;
	}
}

bool unary_operator::on_is_constant() const
{
    return rhs_ != nullptr && rhs_->is_constant();
}

bool compiler::unary_operator::on_emit(gfx::result &r, emit_context_t& context)
{
    auto assembler = context.assembler;
    auto instruction_block = assembler->current_block();
    auto target_reg = assembler->current_target_register();
    auto rhs_reg = register_for(r, context, rhs_);
    if (!rhs_reg.valid) {
        return false;
    }
    auto rhs_size= rhs_reg.size();
    assembler->push_target_register(rhs_size, rhs_reg.i);
    rhs_->emit(r, context);
    assembler->pop_target_register();

    switch (operator_type()) {
        case operator_type_t::negate: {
            instruction_block->neg(rhs_size, target_reg->i, rhs_reg.i);
            break;
        }
        case operator_type_t::binary_not: {
            instruction_block->not_op(rhs_size, target_reg->i, rhs_reg.i);
            break;
        }
        case operator_type_t::logical_not: {
            instruction_block->xor_ireg_by_ireg(rhs_size, target_reg->i, rhs_reg.i, rhs_reg.i);
            break;
        }
        default:
            break;
    }

    return true;
}
void unary_operator::on_owned_elements(element_list_t &list)
{
    if (rhs_ != nullptr) {
        list.emplace_back(rhs_);
    }
}

element *unary_operator::on_fold(result &r, compiler::program *program)
{
    switch (operator_type()) {
        case operator_type_t::negate: {
            break;
        }
        case operator_type_t::binary_not: {
            break;
        }
        case operator_type_t::logical_not: {
            break;
        }
        default:
            break;
    }

    return nullptr;
}
}