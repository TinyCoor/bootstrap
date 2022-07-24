//
// Created by 12132 on 2022/4/23.
//

#include "binary_operator.h"
#include "program.h"
namespace gfx::compiler {
binary_operator::binary_operator(element* parent, operator_type_t type, element* lhs, element* rhs)
	: operator_base(parent, element_type_t::binary_operator,type), lhs_(lhs), rhs_(rhs)
{
}

element* binary_operator::lhs()
{
	return lhs_;
}

element* binary_operator::rhs()
{
	return rhs_;
}
// todo more
compiler::type *binary_operator::on_infer_type(const compiler::program *program)
{
	switch (operator_type()) {
		case operator_type_t::add:
		case operator_type_t::modulo:
		case operator_type_t::divide:
		case operator_type_t::subtract:
		case operator_type_t::multiply:
		case operator_type_t::exponent:
		case operator_type_t::binary_or:
		case operator_type_t::binary_and:
		case operator_type_t::binary_xor:
		case operator_type_t::shift_left:
		case operator_type_t::shift_right:
		case operator_type_t::rotate_left:
		case operator_type_t::rotate_right: {
			// XXX: this is SOOO not correct, but it gets us to the next step of
			//      code generation.
			return program->find_type("u64");
		}
		case operator_type_t::equals:
		case operator_type_t::less_than:
		case operator_type_t::not_equals:
		case operator_type_t::logical_or:
		case operator_type_t::logical_and:
		case operator_type_t::greater_than:
		case operator_type_t::less_than_or_equal:
		case operator_type_t::greater_than_or_equal: {
			return program->find_type("bool");
		}
		default:
			return nullptr;
	}
}

bool binary_operator::on_is_constant() const
{
    return (lhs_ != nullptr && lhs_->is_constant())
        && (rhs_ != nullptr && rhs_->is_constant());
}

bool compiler::binary_operator::on_emit(gfx::result &r, gfx::assembler &assembler, const emit_context_t& context)
{
    auto instruction_block = assembler.current_block();
    switch (operator_type()) {
        case operator_type_t::add:
        case operator_type_t::modulo:
        case operator_type_t::divide:
        case operator_type_t::subtract:
        case operator_type_t::multiply:
        case operator_type_t::exponent:
        case operator_type_t::binary_or:
        case operator_type_t::shift_left:
        case operator_type_t::binary_and:
        case operator_type_t::binary_xor:
        case operator_type_t::shift_right:
        case operator_type_t::rotate_left:
        case operator_type_t::rotate_right: {
            emit_arithmetic_operator(r, assembler, context, instruction_block);
            break;
        }
        case operator_type_t::equals:
        case operator_type_t::less_than:
        case operator_type_t::not_equals:
        case operator_type_t::logical_or:
        case operator_type_t::logical_and:
        case operator_type_t::greater_than:
        case operator_type_t::less_than_or_equal:
        case operator_type_t::greater_than_or_equal: {
            emit_relational_operator(r, assembler, context, instruction_block);
            break;
        }
        case operator_type_t::assignment: {
            auto lhs_reg = instruction_block->allocate_ireg();
            instruction_block->push_target_register(lhs_reg);
            lhs_->emit(r, assembler, emit_context_t::for_write(context));
            instruction_block->pop_target_register();

            auto rhs_reg = instruction_block->allocate_ireg();
            instruction_block->push_target_register(rhs_reg);
            rhs_->emit(r, assembler, emit_context_t::for_read(context));
            instruction_block->pop_target_register();

            int64_t offset = 0;
            auto identifier = dynamic_cast<compiler::identifier*>(lhs_);
            if (identifier->usage() == identifier_usage_t::stack) {
                lhs_reg = i_registers_t::sp;
                offset = -8;
            }

            instruction_block->store_from_ireg<uint64_t>(lhs_reg, rhs_reg, offset);
            instruction_block->pop_target_register();

            instruction_block->free_ireg(lhs_reg);
            instruction_block->free_ireg(rhs_reg);
            break;
        }
        default:
            break;
    }
    return true;
}
void binary_operator::emit_relational_operator(result &r, assembler &assembler,
   const emit_context_t &context, instruction_block *instruction_block)
{
    auto lhs_reg = instruction_block->allocate_ireg();
    instruction_block->push_target_register(lhs_reg);
    lhs_->emit(r, assembler, context);
    instruction_block->pop_target_register();

    auto rhs_reg = instruction_block->allocate_ireg();
    instruction_block->push_target_register(rhs_reg);
    rhs_->emit(r, assembler, context);
    instruction_block->pop_target_register();

    instruction_block->cmp_u64(lhs_reg, rhs_reg);

    switch (operator_type()) {
        case operator_type_t::equals: {
            instruction_block->bne(context.data.if_data->false_branch_label);
            break;
        }
        case operator_type_t::less_than: {
            break;
        }
        case operator_type_t::not_equals: {
            break;
        }
        case operator_type_t::logical_or: {
            break;
        }
        case operator_type_t::logical_and: {
            break;
        }
        case operator_type_t::greater_than: {
            break;
        }
        case operator_type_t::less_than_or_equal: {
            break;
        }
        case operator_type_t::greater_than_or_equal: {
            break;
        }
        default: {
            break;
        }
    }

    instruction_block->free_ireg(lhs_reg);
    instruction_block->free_ireg(rhs_reg);
}
void binary_operator::emit_arithmetic_operator(result &r, assembler &assembler,
    const emit_context_t &context, instruction_block *instruction_block)
{
    auto result_reg = instruction_block->current_target_register();

    auto lhs_reg = instruction_block->allocate_ireg();
    instruction_block->push_target_register(lhs_reg);
    lhs_->emit(r, assembler, context);
    instruction_block->pop_target_register();

    auto rhs_reg = instruction_block->allocate_ireg();
    instruction_block->push_target_register(rhs_reg);
    rhs_->emit(r, assembler, context);
    instruction_block->pop_target_register();

    switch (operator_type()) {
        case operator_type_t::add: {
            instruction_block->add_ireg_by_ireg<uint64_t>(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::divide: {
            instruction_block->div_ireg_by_ireg<uint64_t>(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::modulo: {
            instruction_block->mod_ireg_by_ireg<uint64_t>(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::multiply: {
            instruction_block->mul_ireg_by_ireg<uint64_t>(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::exponent: {
//          nstruction_block->pow_ireg_by_ireg_u64(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::subtract: {
            instruction_block->sub_ireg_by_ireg<uint64_t>(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::binary_or: {
            instruction_block->or_ireg_by_ireg(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::shift_left: {
            instruction_block->shl_ireg_by_ireg(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::binary_and: {
            instruction_block->and_ireg_by_ireg(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::binary_xor: {
            instruction_block->xor_ireg_by_ireg(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::rotate_left: {
            instruction_block->rol_ireg_by_ireg(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::shift_right: {
            instruction_block->shr_ireg_by_ireg(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::rotate_right: {
            instruction_block->ror_ireg_by_ireg(
                result_reg->reg.i,
                lhs_reg,
                rhs_reg);
            break;
        }
        default:
            break;
    }

    instruction_block->free_ireg(lhs_reg);
    instruction_block->free_ireg(rhs_reg);
}

}