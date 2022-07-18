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

bool argument_list::on_emit(result &r, assembler &assembler)
{
    auto instruction_block = assembler.current_block();
    for (auto &arg : elements_) {
        switch (arg->element_type()) {
            case element_type_t::identifier: {
                auto ident = dynamic_cast<compiler::identifier*>(arg);
                auto reg = instruction_block->allocate_ireg();
                instruction_block->move_label_to_ireg(reg, ident->name());
                instruction_block->push_u64(reg);
                instruction_block->free_ireg(reg);
                break;
            }
            case element_type_t::float_literal:
                break;
            case element_type_t::string_literal: {
                auto reg = instruction_block->allocate_ireg();
                instruction_block->move_label_to_ireg(reg, fmt::format("_str_constant_{}", arg->id()));
                instruction_block->push_u64(reg);
                instruction_block->free_ireg(reg);
                break;
            }
            case element_type_t::boolean_literal:
                break;
            case element_type_t::integer_literal:
                break;
            default:
                break;
        }
    }
    return true;
}

}