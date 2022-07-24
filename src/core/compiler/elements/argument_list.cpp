//
// Created by 12132 on 2022/5/28.
//

#include "argument_list.h"
#include "identifier.h"
#include "vm/instruction_block.h"
#include "fmt/format.h"
namespace gfx::compiler {
argument_list::argument_list(element* parent)
	: element(parent, element_type_t::argument_list)
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

bool argument_list::on_emit(result &r, assembler &assembler, const emit_context_t& context)
{
    auto instruction_block = assembler.current_block();
    for (auto &arg : elements_) {
        switch (arg->element_type()) {
            case element_type_t::proc_call:
            case element_type_t::expression:
            case element_type_t::identifier:
            case element_type_t::float_literal:
            case element_type_t::string_literal:
            case element_type_t::boolean_literal:
            case element_type_t::integer_literal:
            case element_type_t::unary_operator:
            case element_type_t::binary_operator: {
                auto target_reg = instruction_block->allocate_ireg();
                instruction_block->push_target_register(target_reg);
                arg->emit(r, assembler, context);
                instruction_block->pop_target_register();
                instruction_block->push<uint64_t>(target_reg);
                instruction_block->free_ireg(target_reg);
                break;
            }
            default:
                break;
        }
    }
    return true;
}

}