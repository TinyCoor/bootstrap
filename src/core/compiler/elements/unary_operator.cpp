//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
unary_operator::unary_operator(compiler::module* module, block* parent, operator_type_t type,element* rhs)
	: operator_base(module, parent, element_type_t::unary_operator, type), rhs_(rhs)
{
}

element* unary_operator::rhs()
{
	return rhs_;
}

bool unary_operator::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
	switch (operator_type()) {
		case operator_type_t::negate:
		case operator_type_t::binary_not: {
			result.type = session.scope_manager().find_type(qualified_symbol_t{.name= "u64"});
            return result.type !=nullptr;
        }
		case operator_type_t::logical_not: {
			result.type = session.scope_manager().find_type(qualified_symbol_t{.name = "bool"});
            return result.type !=nullptr;
        }
		default:
			return false;
	}
}

bool unary_operator::on_is_constant() const
{
    return rhs_ != nullptr && rhs_->is_constant();
}

bool compiler::unary_operator::on_emit(compiler::session &session)
{
    auto &assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    auto target_reg = assembler.current_target_register();
    auto rhs_reg = register_for(session, rhs_);
    if (!rhs_reg.valid) {
        return false;
    }
    assembler.push_target_register(rhs_reg.reg);
    rhs_->emit(session);
    assembler.pop_target_register();

    switch (operator_type()) {
        case operator_type_t::negate: {
            instruction_block->neg(*target_reg, rhs_reg.reg);
            break;
        }
        case operator_type_t::binary_not: {
            instruction_block->not_op(*target_reg, rhs_reg.reg);
            break;
        }
        case operator_type_t::logical_not: {
            instruction_block->xor_reg_by_reg(*target_reg, rhs_reg.reg, rhs_reg.reg);
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

bool unary_operator::on_fold(compiler::session& session, fold_result_t& result)
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

    return true;
}
void unary_operator::rhs(compiler::element *element)
{
    rhs_ = element;
}
}