//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_LABEL_H_
#define COMPILER_ELEMENTS_LABEL_H_
#include "element.h"
namespace gfx::compiler {
class label : public element {
public:
	label(compiler::module* module, block* parent_scope, const std::string& name);

	std::string name() const;
private:
	std::string name_;
};
}

#endif // COMPILER_ELEMENTS_LABEL_H_
