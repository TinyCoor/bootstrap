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
    module(compiler::module* module, compiler::block* parent_scope, compiler::block* scope);

    compiler::block* scope();

    bool is_root() const;

    void is_root(bool value);

    gfx::source_file* source_file() const;

    void source_file(gfx::source_file* value);

protected:
    bool on_emit(compiler::session &session) override;

    void on_owned_elements(gfx::compiler::element_list_t &list) override;

private:
    bool is_root_ = false;
    block* scope_ = nullptr;
    gfx::source_file *source_file_ = nullptr;

};
}

#endif // COMPILER_ELEMENTS_MODULE_H_
