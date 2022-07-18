//
// Created by 12132 on 2022/4/20.
//

#include "assembler.h"

namespace gfx {

assembler::assembler(terp* terp)
	: terp_(terp)
{
	location_counter_ = terp_->heap_vector(heap_vectors_t::program_start);
}

assembler::~assembler()
{
    for (auto &block : blocks_) {
        delete block;
    }
    blocks_.clear();
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
    return current_block_;
}

instruction_block *assembler::make_new_block()
{
    auto block = new instruction_block();
    blocks_.push_back(block);
    return block;
}

gfx::segment *assembler::segment(const std::string &name, segment_type_t type)
{
    segments_.insert(std::make_pair(name, gfx::segment(name, type)));
    return segment(name);
}
}