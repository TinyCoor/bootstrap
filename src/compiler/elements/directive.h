//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_DIRECTIVE_H_
#define COMPILER_ELEMENTS_DIRECTIVE_H_
#include "expression.h"
namespace gfx::compiler {

class directive : public element {
public:
	directive(const std::string& name, expression* rhs);

	~directive() override;

	expression* rhs();

	std::string name() const;

private:
	std::string name_;
	expression* rhs_ = nullptr;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_DIRECTIVE_H_
