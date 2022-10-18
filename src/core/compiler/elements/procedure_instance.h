//
// Created by 12132 on 2022/5/8.
//

#ifndef COMPILER_ELEMENTS_PROCEDURE_INSTANCE_H_
#define COMPILER_ELEMENTS_PROCEDURE_INSTANCE_H_
#include "element.h"
namespace gfx::compiler {
class procedure_instance : public element {
public:
	procedure_instance(compiler::module* module, block *parent_scope, compiler::type *procedure_type, block *scope);

	block *scope();

	compiler::type *procedure_type();
protected:
    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

private:
	block * scope_ = nullptr;
	compiler::type *procedure_type_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_PROCEDURE_INSTANCE_H_
