//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_FIELD_H_
#define COMPILER_ELEMENTS_FIELD_H_
#include <map>
#include <string>
#include <memory>
#include "type.h"
#include "element.h"
#include "initializer.h"
namespace gfx::compiler {
class field : public element {
public:
	field(const std::string& name, type* type, initializer* initializer);

	~field() override;

	compiler::type* type();

	std::string name() const;

	bool inferred_type() const;

	void type(compiler::type* t);

	compiler::initializer* initializer();

	void initializer(compiler::initializer* v);

private:
	std::string name_;
	bool inferred_type_ = false;
	compiler::type* type_ = nullptr;
	compiler::initializer* initializer_ = nullptr;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_FIELD_H_
