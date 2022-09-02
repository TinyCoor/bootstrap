//
// Created by 12132 on 2022/4/20.
//

#include "assembler.h"

namespace gfx {

assembler::assembler(terp* terp)
	: terp_(terp)
{
}

assembler::~assembler()
{
    for (auto &block : blocks_) {
        delete block;
    }
    blocks_.clear();
}

bool assembler::initialize(result &r)
{
    location_counter_ = terp_->heap_vector(heap_vectors_t::program_start);
    return true;
}
bool assembler::assemble(result &r, instruction_block *block)
{
    return false;
}

bool assembler::assemble_from_source(result &r, std::istream &source)
{
	return false;
}

gfx::segment *assembler::segment(const std::string &name)
{
	auto it = segments_.find(name);
	if (it == segments_.end()) {
		return nullptr;
	}
	return &it->second;
}

segment_list_t assembler::segments() const
{
    segment_list_t list {};
    for (const auto& it: segments_) {
        list.push_back(it.second);
    }
    return list;
}

instruction_block *assembler::current_block()
{
    if (block_stack_.empty()) {
        return nullptr;
    }
    return block_stack_.top();
}

gfx::segment *assembler::segment(const std::string &name, segment_type_t type)
{
    segments_.insert(std::make_pair(name, gfx::segment(name, type)));
    return segment(name);
}

void assembler::push_block(instruction_block *block)
{
    if (block->type() == instruction_block_type_t::procedure) {
        procedure_block_count_++;
    }
    block_stack_.push(block);
}

instruction_block *assembler::pop_block()
{
    if (block_stack_.empty()) {
        return nullptr;
    }
    auto top = block_stack_.top();
    if (top->type() == instruction_block_type_t::procedure && procedure_block_count_ > 0) {
        procedure_block_count_--;
    }
    block_stack_.pop();
    return top;
}

instruction_block *assembler::make_basic_block(instruction_block* parent)
{
    auto block = new instruction_block(parent ==nullptr ? current_block() : parent,
        instruction_block_type_t::basic);
    add_new_block(block);
    return block;
}

instruction_block *assembler::make_procedure_block(instruction_block* parent)
{
    auto block = new instruction_block(parent == nullptr ? current_block() : parent ,
        instruction_block_type_t::procedure);
    add_new_block(block);
    return block;
}

void assembler::add_new_block(instruction_block *block)
{
    auto source_file = listing_.current_source_file();
    if (source_file != nullptr) {
        block->source_file(source_file);
    }
    blocks_.push_back(block);
    auto top_block = current_block();
    if (top_block != nullptr) {
        top_block->add_block(block);
    }
}
instruction_block *assembler::root_block()
{
    return blocks_.front();
}

bool assembler::in_procedure_scope() const
{
    return procedure_block_count_ > 0;
}

assembly_listing &assembler::listing()
{
    return listing_;
}
}