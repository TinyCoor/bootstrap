//
// Created by 12132 on 2022/7/2.
//

#ifndef BOOTSTRAP_COMPILER_ELEMENTS_IMPORT_H_
#define BOOTSTRAP_COMPILER_ELEMENTS_IMPORT_H_
#include "element.h"
namespace gfx::compiler {
	class import : public element {
	public:
		import(block* parent_scope, element* expr);

		element* expression();

    protected:
        void on_owned_elements(element_list_t& list) override;

	private:
		element* expression_ = nullptr;
	};
}

#endif // BOOTSTRAP_COMPILER_ELEMENTS_IMPORT_H_
