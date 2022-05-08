//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_DIRECTIVE_H_
#define COMPILER_ELEMENTS_DIRECTIVE_H_
#include "block.h"
namespace gfx::compiler {

class directive : public element {
public:
	directive(block* parent, const std::string& name, element* expression);

	element* expression();

	std::string name() const;

private:
	std::string name_;
	element* expression_ = nullptr;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_DIRECTIVE_H_
