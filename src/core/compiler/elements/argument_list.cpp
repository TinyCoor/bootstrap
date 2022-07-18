//
// Created by 12132 on 2022/5/28.
//

#include "argument_list.h"
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
        uint64_t value = 0;
        instruction_block->push(value);
    }
    return true;
}

}