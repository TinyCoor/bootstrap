//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ATTRIBUTE_H_
#define COMPILER_ELEMENTS_ATTRIBUTE_H_
#include <string>
#include "element.h"

namespace gfx::compiler {
class attribute : public element{
public:
	attribute(const std::string& name, expression* rhs);

	~attribute() override;

private:
	std::string name_;
	expression* rhs_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_ATTRIBUTE_H_
