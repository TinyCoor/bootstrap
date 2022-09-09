//
// Created by 12132 on 2022/8/31.
//

#ifndef COMPILER_ELEMENTS_MODULE_H_
#define COMPILER_ELEMENTS_MODULE_H_
#include "element.h"
#include "common/source_file.h"
namespace gfx::compiler {
namespace fs = std::filesystem;
class module : public element {
public:
    module(compiler::block* parent_scope, compiler::block* scope);

    compiler::block* scope();

    gfx::source_file* source_file() const;

    void source_file(gfx::source_file* value);

protected:
    void on_owned_elements(gfx::compiler::element_list_t &list) override;

    bool on_emit(gfx::result &r, gfx::compiler::emit_context_t &context) override;
private:
    block* scope_ = nullptr;
    gfx::source_file *source_file_ = nullptr;

};
}

#endif // COMPILER_ELEMENTS_MODULE_H_
