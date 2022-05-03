//
// Created by 12132 on 2022/4/23.
//

#include "block.h"
namespace gfx::compiler {
block::block(block* parent) : element(parent)
{
}

block::~block()
{
}

element_list_t& block::children()
{
	return children_;
}


}