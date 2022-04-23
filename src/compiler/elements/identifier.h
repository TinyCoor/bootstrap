//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_IDENTIFIER_H_
#define COMPILER_ELEMENTS_IDENTIFIER_H_

#include "type.h"
#include "element.h"
#include "initializer.h"
namespace gfx::compiler {

class identifier : public element {
public:
	identifier(const std::string& name, const compiler::initializer& initializer);

	~identifier() override;

	compiler::type* type();

	std::string name() const;

	bool is_constant() const;

	bool inferred_type() const;

	void type(compiler::type* t);

	const compiler::initializer& initializer() const;

private:
	std::string name_;
	bool constant_ = false;
	bool inferred_type_ = false;
	compiler::type* type_ = nullptr;
	compiler::initializer initializer_;
};
}
#endif // COMPILER_ELEMENTS_IDENTIFIER_H_
