//
// Created by 12132 on 2022/7/2.
//

#ifndef COMPILER_ELEMENTS_IMPORT_H_
#define COMPILER_ELEMENTS_IMPORT_H_
#include "element.h"
namespace gfx::compiler {
	class import : public element {
	public:
		import(block* parent_scope, element* expr, element* from_expr);

		element* expression();

        element* from_expression();

    protected:
        void on_owned_elements(element_list_t& list) override;

	private:
		element* expression_ = nullptr;
        element* from_expression_ = nullptr;
	};
}

#endif // COMPILER_ELEMENTS_IMPORT_H_
