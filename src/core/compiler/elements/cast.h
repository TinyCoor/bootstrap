//
// Created by 12132 on 2022/5/8.
//

#ifndef COMPILER_ELEMENTS_CAST_H_
#define COMPILER_ELEMENTS_CAST_H_

#include "element.h"
namespace gfx::compiler {
class cast : public element {
public:
	cast(block* parent_scope, compiler::type* type, element* expr);

	compiler::type* type();

	element* expression();

protected:
	compiler::type* on_infer_type(const compiler::program* program) override;

    bool on_emit(result& r, emit_context_t& context) override;

private:
	element* expression_ = nullptr;
	compiler::type* type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_CAST_H_
