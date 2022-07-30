//
// Created by 12132 on 2022/5/14.
//

#ifndef COMPILER_ELEMENTS_PROCEDURE_CALL_H_
#define COMPILER_ELEMENTS_PROCEDURE_CALL_H_
#include "argument_list.h"
namespace gfx::compiler {
class procedure_call : public element {
public:
	procedure_call(element* parent, identifier* identifier, argument_list* args);

	identifier* identifier();

	argument_list* arguments();

protected:
    bool on_emit(result& r, emit_context_t& context)  override;

	compiler::type* on_infer_type(const compiler::program* program) override;

private:
	compiler::argument_list* arguments_ = nullptr;
	compiler::identifier* identifier_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_PROCEDURE_CALL_H_
