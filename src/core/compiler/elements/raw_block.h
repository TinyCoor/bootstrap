//
// Created by 12132 on 2022/10/24.
//

#ifndef COMPILER_ELEMENTS_RAW_BLOCK_H_
#define COMPILER_ELEMENTS_RAW_BLOCK_H_
#include "element.h"
namespace gfx::compiler {
class raw_block : public element {
public:
    raw_block(compiler::module* module, block* parent_scope,
        const std::string& value);

    std::string value() const;

private:
    std::string value_;

};
}

#endif // COMPILER_ELEMENTS_RAW_BLOCK_H_
