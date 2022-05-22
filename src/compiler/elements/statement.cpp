//
// Created by 12132 on 2022/5/8.
//

#include "statement.h"
namespace gfx::compiler {
statement::statement(element* parent, element* expr)
	: element(parent, element_type_t::statement),
					 expression_(expr) {
}

element* statement::expression() {
	return expression_;
}

label_list_t& statement::labels() {
	return labels_;
}
}