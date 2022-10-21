//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_PROGRAM_H_
#define COMPILER_ELEMENTS_PROGRAM_H_
#include "block.h"
#include "common/source_file.h"

namespace gfx::compiler {

class program : public element {
public:
    program();

    program(const program& other) = delete;

	~program() override;

    compiler::block *block() const;

    void block(compiler::block* value);

private:
    bool on_emit(compiler::session &session) override;

private:
	compiler::block* block_ = nullptr;

};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
