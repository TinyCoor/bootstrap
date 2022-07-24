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
	namespace_element(element* parent,  element* expr);

	element* expression();

protected:
	compiler::type* on_infer_type(const compiler::program* program) override;
    bool on_is_constant() const override;

private:
	element* expression_ = nullptr;
};

}
#endif // COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
