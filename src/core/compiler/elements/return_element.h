
//
// Created by 12132 on 2022/5/14.
//

#ifndef COMPILER_ELEMENTS_RETURN_ELEMENT_H_
#define COMPILER_ELEMENTS_RETURN_ELEMENT_H_
#include "element.h"
namespace gfx::compiler {
class return_element : public element {
public:
	 explicit return_element(element* parent);

	element_list_t& expressions();
protected:
    bool on_emit(result &r, emit_context_t &context ) override;

private:
	element_list_t expressions_ {};
};
}

#endif // COMPILER_ELEMENTS_RETURN_ELEMENT_H_
