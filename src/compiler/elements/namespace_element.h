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

private:
	element* expression_ = nullptr;
};

}
#endif // COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
