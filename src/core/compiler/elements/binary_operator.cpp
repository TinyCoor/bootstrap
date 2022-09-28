//
// Created by 12132 on 2022/4/23.
//

#include "binary_operator.h"
#include "symbol_element.h"
#include "integer_literal.h"
#include "program.h"
#include "fmt/format.h"
namespace gfx::compiler {
binary_operator::binary_operator(block* parent, operator_type_t type, element* lhs, element* rhs)
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
			return program->find_type(qualified_symbol_t{.name = "u64"});
		}
		case operator_type_t::equals:
		case operator_type_t::less_than:
		case operator_type_t::not_equals:
		case operator_type_t::logical_or:
		case operator_type_t::logical_and:
		case operator_type_t::greater_than:
		case operator_type_t::less_than_or_equal:
		case operator_type_t::greater_than_or_equal: {
			return program->find_type(qualified_symbol_t{.name = "bool"});
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

bool compiler::binary_operator::on_emit(gfx::result &r, emit_context_t& context)
{
    auto assembler= context.assembler;
    auto instruction_block = assembler->current_block();
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
            emit_arithmetic_operator(r, context, instruction_block);
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
            emit_relational_operator(r, context, instruction_block);
            break;
        }
        case operator_type_t::assignment: {
            auto var = context.variable_for_element(lhs_);
            if (var == nullptr) {
                context.program->error(r, lhs_, "P051", fmt::format("missing assembler variable for {}.", lhs_->label_name()),
                    lhs_->location());
                return false;
            }
            var->make_live(context.assembler);
            lhs_->emit(r, context);
            var->init(context.assembler, instruction_block);

            i_registers_t rhs_reg;
            if (!assembler->allocate_reg(rhs_reg)) {
                context.program->error(r, this, "P052", "assembler registers exhausted.", location());
            }

            assembler->push_target_register(rhs_reg);
            rhs_->emit(r, context);
            var->write(assembler, instruction_block);
            assembler->pop_target_register();
            assembler->free_reg(rhs_reg);
            var->make_dormat(assembler);
            break;
        }
        default:
            break;
    }
    return true;
}

void binary_operator::emit_relational_operator(result &r, emit_context_t &context, instruction_block *instruction_block)
{
    auto &assembler = context.assembler;
    auto lhs_reg = register_for(r, context, lhs_);
    auto rhs_reg = register_for(r, context, rhs_);
    if (!lhs_reg.valid || !rhs_reg.valid) {
        return;
    }

    assembler->push_target_register(lhs_reg.reg.i);
    lhs_->emit(r, context);
    assembler->pop_target_register();

    assembler->push_target_register(rhs_reg.reg.i);
    rhs_->emit(r, context);
    assembler->pop_target_register();

    auto if_data = context.top<if_data_t>();
    switch (operator_type()) {
        case operator_type_t::equals: {
            instruction_block->cmp(op_sizes::qword, lhs_reg.reg.i, rhs_reg.reg.i);
            if (if_data != nullptr) {
                auto parent_op = parent_element_as<compiler::binary_operator>();
                if (parent_op !=nullptr && parent_op->operator_type() == operator_type_t::logical_and) {
                    instruction_block->bne(if_data->false_branch_label);
                } else {
                    instruction_block->beq(if_data->true_branch_label);
                }
            } else {
                auto target_reg = assembler->current_target_register();
                instruction_block->setz(target_reg->reg.i);
                context.push_scratch_register(target_reg->reg.i);
            }
            break;
        }
        case operator_type_t::less_than: {
            break;
        }
        case operator_type_t::not_equals: {
            break;
        }
        case operator_type_t::logical_or: {
            if (if_data != nullptr) {
                instruction_block->jump_direct(if_data->false_branch_label);
            } else {
                auto lhs_target_reg = context.pop_scratch_register();
                auto rhs_target_reg =  context.pop_scratch_register();
                auto target_reg = assembler->current_target_register();
                instruction_block->or_ireg_by_ireg(op_sizes::qword, target_reg->reg.i, lhs_target_reg,
                    rhs_target_reg);
            }
            break;
        }
        case operator_type_t::logical_and: {
            if (if_data != nullptr) {
                instruction_block->jump_direct(if_data->true_branch_label);
            } else {
                auto rhs_target_reg = context.pop_scratch_register();
                auto lhs_target_reg = context.pop_scratch_register();
                auto target_reg = assembler->current_target_register();
                instruction_block->and_ireg_by_ireg(op_sizes::qword, target_reg->reg.i, lhs_target_reg,
                    rhs_target_reg);
            }
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
}

void binary_operator::emit_arithmetic_operator(result &r, emit_context_t &context, instruction_block *instruction_block)
{
    auto assembler = context.assembler;
    auto result_reg = assembler->current_target_register();
    auto lhs_reg = register_for(r, context, lhs_);
    auto rhs_reg = register_for(r, context, rhs_);
    if (!lhs_reg.valid || !rhs_reg.valid) {
        return;
    }
    assembler->push_target_register(lhs_reg.reg.i);
    lhs_->emit(r, context);
    assembler->pop_target_register();
    assembler->push_target_register(rhs_reg.reg.i);
    rhs_->emit(r, context);
    assembler->pop_target_register();

    switch (operator_type()) {
        case operator_type_t::add: {
            instruction_block->add_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::divide: {
            instruction_block->div_ireg_by_ireg(op_sizes::qword,result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::modulo: {
            instruction_block->mod_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::multiply: {
            instruction_block->mul_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::exponent: {
//          nstruction_block->pow_ireg_by_ireg_u64(result_reg->reg.i, lhs_reg, rhs_reg);
            break;
        }
        case operator_type_t::subtract: {
            instruction_block->sub_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::binary_or: {
            instruction_block->or_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::shift_left: {
            instruction_block->shl_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::binary_and: {
            instruction_block->and_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::binary_xor: {
            instruction_block->xor_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::rotate_left: {
            instruction_block->rol_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::shift_right: {
            instruction_block->shr_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        case operator_type_t::rotate_right: {
            instruction_block->ror_ireg_by_ireg(op_sizes::qword, result_reg->reg.i, lhs_reg.reg.i, rhs_reg.reg.i);
            break;
        }
        default:
            break;
    }
}

element *binary_operator::on_fold(result &r, compiler::program *program)
{
    switch (operator_type()) {
        case operator_type_t::add: {
            break;
        }
        case operator_type_t::divide: {
            break;
        }
        case operator_type_t::modulo: {
            break;
        }
        case operator_type_t::equals: {
            break;
        }
        case operator_type_t::subtract: {
            break;
        }
        case operator_type_t::exponent: {
            break;
        }
        case operator_type_t::multiply: {
            break;
        }
        case operator_type_t::binary_or: {
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
        case operator_type_t::binary_and: {
            break;
        }
        case operator_type_t::binary_xor: {
            break;
        }
        case operator_type_t::shift_left: {
            break;
        }
        case operator_type_t::logical_and: {
            break;
        }
        case operator_type_t::shift_right: {
            break;
        }
        case operator_type_t::rotate_left: {
            break;
        }
        case operator_type_t::rotate_right: {
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
        default:
            break;
    }

    return nullptr;
}

void binary_operator::on_owned_elements(element_list_t &list)
{
    if (lhs_ != nullptr) {
        list.emplace_back(lhs_);
    }

    if (rhs_ != nullptr) {
        list.emplace_back(rhs_);
    }
}


}