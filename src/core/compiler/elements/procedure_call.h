//
// Created by 12132 on 2022/5/14.
//

#ifndef COMPILER_ELEMENTS_PROCEDURE_CALL_H_
#define COMPILER_ELEMENTS_PROCEDURE_CALL_H_
#include "argument_list.h"
namespace gfx::compiler {
class procedure_call : public element {
public:
	procedure_call(compiler::module* module,block* parent_scope, compiler::identifier_reference* reference,
                   argument_list* args);

	argument_list* arguments();

    compiler::identifier_reference* reference();

    void reference(compiler::identifier_reference* value);

protected:
    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;


private:
	compiler::argument_list* arguments_ = nullptr;
    compiler::identifier_reference* reference_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_PROCEDURE_CALL_H_
