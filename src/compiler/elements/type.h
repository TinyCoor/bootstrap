//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_TYPE_H_
#define COMPILER_ELEMENTS_TYPE_H_
#include <string>
#include "element.h"
#include "element_types.h"
namespace gfx::compiler {
class type : public element {
public:
	explicit type(element* parent, element_type_t type, const std::string& name);

	~type() override;

	std::string name() const;

	void name(const std::string& value);

private:
	std::string name_;
};

}
#endif // COMPILER_ELEMENTS_TYPE_H_
