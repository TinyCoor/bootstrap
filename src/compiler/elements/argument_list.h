//
// Created by 12132 on 2022/5/28.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_ARGUMENT_LIST_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_ARGUMENT_LIST_H_

#include "element.h"

namespace gfx::compiler {

class argument_list : public element {
public:
	argument_list(element* parent);

	void add(element* item);

	void remove(id_t id);

	element* find(id_t id);

	const element_list_t& elements() const;

private:
	element_list_t elements_ {};
};

};


#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_ARGUMENT_LIST_H_
