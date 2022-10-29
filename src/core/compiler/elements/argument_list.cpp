//
// Created by 12132 on 2022/5/28.
//

#include "argument_list.h"
#include "types/type.h"
#include "program.h"
#include "core/compiler/session.h"
#include "vm/instruction_block.h"
#include "fmt/format.h"
namespace gfx::compiler {
argument_list::argument_list(compiler::module* module, block* parent)
	: element(module, parent, element_type_t::argument_list)
{
}

void argument_list::add(element* item)
{
	elements_.emplace_back(item);
}

void argument_list::remove(id_t id)
{
	auto item = find(id);
	if (item == nullptr) {
		return;
	}

	(void)std::remove(elements_.begin(), elements_.end(), item);
}

element* argument_list::find(id_t id)
{
	auto it = std::find_if(elements_.begin(), elements_.end(),
		[&id](auto item) { return item->id() == id; });
	if (it == elements_.end()) {
		return nullptr;
	}
	return *it;
}

const element_list_t& argument_list::elements() const
{
	return elements_;
}

bool argument_list::on_emit(compiler::session& session)
{
    auto &assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    for (auto it = elements_.rbegin(); it != elements_.rend(); ++it) {
        element* arg = *it;
        if (arg->element_type() == element_type_t::intrinsic) {
            auto folded_expr = arg->fold(session);
            if (folded_expr != nullptr) {
                instruction_block->blank_line();
                instruction_block->comment("intrinsic constant fold", 4);
                arg = folded_expr;
            }
        }
        switch (arg->element_type()) {
            case element_type_t::proc_call:
            case element_type_t::expression:
            case element_type_t::float_literal:
            case element_type_t::string_literal:
            case element_type_t::boolean_literal:
            case element_type_t::integer_literal:
            case element_type_t::unary_operator:
            case element_type_t::binary_operator:
            case element_type_t::identifier_reference:{
                auto arg_reg = register_for(session, arg);
                if (arg_reg.var != nullptr) {
                    arg_reg.clean_up = true;
                }
                assembler.push_target_register(arg_reg.reg);
                arg->emit(session);
                assembler.pop_target_register();
                instruction_block->push(arg_reg.reg);
                break;
            }
            default:
                break;
        }
    }
    return true;
}
void argument_list::on_owned_elements(element_list_t &list)
{
    for (auto element : elements_) {
        list.emplace_back(element);
    }
}

}