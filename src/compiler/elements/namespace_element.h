//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
#define COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
#include "element_types.h"
#include "block.h"
namespace gfx::compiler {
class namespace_element : public block {
public:
	namespace_element(block* parent, const std::string& name);

	~namespace_element() override;

	std::string name() const;

private:
	std::string name_;
};

}
#endif // COMPILER_ELEMENTS_NAMESPACE_ELEMENT_H_
