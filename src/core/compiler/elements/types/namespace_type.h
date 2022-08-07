//
// Created by 12132 on 2022/5/22.
//

#ifndef COMPILER_ELEMENTS_NAMESPACE_TYPE_H_
#define COMPILER_ELEMENTS_NAMESPACE_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class namespace_type : public type {
public:
	explicit namespace_type(block* parent_scope);

protected:
	bool on_initialize(result& r, compiler::program* program) override;
private:
};
}

#endif // COMPILER_ELEMENTS_NAMESPACE_TYPE_H_
