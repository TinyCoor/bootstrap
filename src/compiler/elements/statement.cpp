//
// Created by 12132 on 2022/5/8.
//

#include "statement.h"
namespace gfx::compiler {
statement::statement(element* parent, element* expr)
	: element(parent, element_type_t::statement),
					 expr_(expr) {
}

element* statement::expr() {
	return expr_;
}

label_list_t& statement::labels() {
	return labels_;
}
}