//
// Created by 12132 on 2022/7/30.
//

#include "stack_frame.h"
#include "instruction_block.h"
namespace gfx {
stack_frame_t::stack_frame_t(instruction_block *parent_block)
    : parent_block_(parent_block)
{
}

stack_frame_entry_t *stack_frame_t::add(stack_frame_entry_type_t type,
                                        const std::string &name, int32_t offset)
{
    auto it = entries.insert(std::make_pair(name,
         stack_frame_entry_t{
              .offset = offset,
              .name = name,
              .type = type}));
    return &it.first->second;
}

stack_frame_entry_t *stack_frame_t::find(const std::string &name)
{
    auto it = entries.find(name);
    if (it == entries.end()) {
        return nullptr;
    }
    return &it->second;
}

stack_frame_entry_t *stack_frame_t::find_up(const std::string &name)
{
    auto current_frame = this;
    while (current_frame != nullptr) {
        auto entry = current_frame->find(name);
        if (entry != nullptr) {
            return entry;
        }
        current_frame = current_frame->parent_block_->stack_frame();
    }
    return nullptr;
}
}