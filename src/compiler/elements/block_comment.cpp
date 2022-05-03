//
// Created by 12132 on 2022/5/3.
//

#include "block_comment.h"
namespace gfx::compiler {
block_comment::block_comment(element *parent, const std::string &value)
	: element(parent),value_(value)
{

}
}