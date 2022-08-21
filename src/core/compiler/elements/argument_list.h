//
// Created by 12132 on 2022/5/28.
//

#ifndef COMPILER_ELEMENTS_ARGUMENT_LIST_H_
#define COMPILER_ELEMENTS_ARGUMENT_LIST_H_

#include "element.h"

namespace gfx::compiler {
class argument_list : public element {
public:
	explicit argument_list(block* parent_scope);

	void add(element* item);

	void remove(id_t id);

	element* find(id_t id);

	const element_list_t& elements() const;
protected:
    void on_owned_elements(element_list_t& list) override;

    bool on_emit(result& r, emit_context_t& context) override;

private:
	element_list_t elements_ {};
};
}
#endif // COMPILER_ELEMENTS_ARGUMENT_LIST_H_
