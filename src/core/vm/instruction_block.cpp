//
// Created by 12132 on 2022/7/17.
//

#include "instruction_block.h"
namespace gfx {

instruction_block::instruction_block() {
}

instruction_block::~instruction_block()
{
    for (const auto& it : labels_){
        delete it.second;
    }
    labels_.clear();
}

void instruction_block::push(double value)
{
}

void instruction_block::push(uint64_t value)
{

}

void instruction_block::call(const std::string& proc_name)
{

}

label* instruction_block::make_label(const std::string& name)
{
    auto label = new class label(name);
    labels_.insert(std::make_pair(name, label));
    label_to_instruction_map_.insert(std::make_pair(name, instructions_.size()));
    return label;
}

}