//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INITIALIZER_H_
#define COMPILER_ELEMENTS_INITIALIZER_H_
#include "element.h"

namespace gfx::compiler {
class initializer : public element {
public:
	explicit initializer(compiler::module* module, block* parent_scope, element* expr);

	element* expression();

    void expression(element* value);

	compiler::procedure_type* procedure_type();
protected:
    bool on_emit(compiler::session &session) override;

    bool on_as_bool(bool &value) const override;

    bool on_as_float(double &value) const override;

    bool on_as_integer(uint64_t &value) const override;

    bool on_as_string(std::string &value) const override;

    void on_owned_elements(element_list_t &list) override;

    compiler::type* on_infer_type(const compiler::session& session) override;
private:
	element* expr_ = nullptr;
};
}

#endif //COMPILER_ELEMENTS_INITIALIZER_H_
