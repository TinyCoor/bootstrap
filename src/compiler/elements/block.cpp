//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
namespace gfx::compiler {
block::block(block* parent) : parent_(parent)
{
}

block::~block()
{
}

block* block::parent() const
{
	return parent_;
}

element_list_t& block::children()
{
	return children_;
}


}