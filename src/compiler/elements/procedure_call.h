//
// Created by 12132 on 2022/5/14.
//

#ifndef COMPILER_ELEMENTS_PROCEDURE_CALL_H_
#define COMPILER_ELEMENTS_PROCEDURE_CALL_H_
#include "element.h"
namespace gfx::compiler {
class procedure_call : public element {
public:
	procedure_call(element* parent, compiler::type* procedure_type, element* expr);

	element* expression();

	compiler::type* procedure_type();

private:
	element* expression_ = nullptr;
	compiler::type* procedure_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_PROCEDURE_CALL_H_
