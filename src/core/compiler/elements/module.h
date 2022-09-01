//
// Created by 12132 on 2022/8/31.
//

#ifndef COMPILER_ELEMENTS_MODULE_H_
#define COMPILER_ELEMENTS_MODULE_H_
#include "element.h"
namespace gfx::compiler {
namespace fs = std::filesystem;
class module : public element {
public:
    module(compiler::block* parent_scope, compiler::block* scope);

    compiler::block* scope();

    const fs::path& source_file() const;

    void source_file(const fs::path& value);

protected:
    void on_owned_elements(gfx::compiler::element_list_t &list) override;

    bool on_emit(gfx::result &r, gfx::compiler::emit_context_t &context) override;
private:
    block* scope_ = nullptr;
    fs::path source_file_;

};
}

#endif // COMPILER_ELEMENTS_MODULE_H_
