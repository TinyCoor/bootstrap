//
// Created by 12132 on 2022/7/2.
//

#ifndef BOOTSTRAP_COMPILER_ELEMENTS_IMPORT_H_
#define BOOTSTRAP_COMPILER_ELEMENTS_IMPORT_H_
#include "element.h"
namespace gfx::compiler {
	class import : public element {
	public:
		import(element* parent, element* expr);

		element* expression();

	private:
		element* expression_ = nullptr;
	};
}

#endif // BOOTSTRAP_COMPILER_ELEMENTS_IMPORT_H_
