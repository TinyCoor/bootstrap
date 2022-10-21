//
// Created by 12132 on 2022/5/8.
//

#ifndef COMPILER_ELEMENTS_CAST_H_
#define COMPILER_ELEMENTS_CAST_H_

#include "element.h"
namespace gfx::compiler {
class cast : public element {
public:
	cast(compiler::module* module, block* parent_scope, compiler::type* type, element* expr);

	compiler::type* type();

	element* expression();

protected:
    void on_owned_elements(element_list_t& list) override;

    bool on_emit(compiler::session& session) override;

    compiler::type* on_infer_type(const compiler::session& session) override;

private:
	element* expression_ = nullptr;
	compiler::type* type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_CAST_H_
