//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
namespace gfx::compiler {
block::block(block* parent,element_type_t type)
	: element(parent, type), emitter_(0)
{
}

block::~block()
{
}

bool block::emit(result& r)
{
	emitter_.clear();
	return false;
}

instruction_emitter* block::emitter()
{
	return &emitter_;
}

type_map_t& block::types()
{
	return types_;
}

identifier_map_t& block::identifiers()
{
	return identifiers_;
}

statement_list_t &block::statements()
{
	return statements_;
}

comment_list_t &block::comments()
{
	return comments_;
}

block_list_t &block::blocks()
{
	return blocks_;
}

}