//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_IDENTIFIER_H_
#define COMPILER_ELEMENTS_IDENTIFIER_H_

#include "element.h"

namespace gfx::compiler {

class identifier : public element {
public:
	identifier(element* parent, const std::string& name, compiler::initializer* initializer);

	compiler::type* type();

	std::string name() const;

	bool constant() const;

	bool inferred_type() const;

	void constant(bool value);

	void type(compiler::type* t);

	void inferred_type(bool value);

	compiler::initializer* initializer() const;

private:
	std::string name_;
	bool constant_ = false;
	bool inferred_type_ = false;
	compiler::type* type_ = nullptr;
	compiler::initializer* initializer_;
};
}
#endif // COMPILER_ELEMENTS_IDENTIFIER_H_
