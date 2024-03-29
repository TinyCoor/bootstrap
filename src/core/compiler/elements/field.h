//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_FIELD_H_
#define COMPILER_ELEMENTS_FIELD_H_

#include "element.h"
#include "initializer.h"
namespace gfx::compiler {
class field : public element {
public:
	field(compiler::module* module,block* parent_scope, compiler::identifier* identifier);

	compiler::identifier* identifier();

protected:
    void on_owned_elements(element_list_t& list) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;
private:
	compiler::identifier* identifier_;
};

}

#endif // COMPILER_ELEMENTS_FIELD_H_
