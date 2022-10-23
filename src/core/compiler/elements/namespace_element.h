//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
#define COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
#include "element_types.h"
#include "block.h"
namespace gfx::compiler {
class namespace_element : public element {
public:
	namespace_element(compiler::module* module, block* parent_scope, element* expr);

	element* expression();

    std::string name();

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;

private:
	element* expression_ = nullptr;
};

}
#endif // COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
