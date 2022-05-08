//
// Created by 12132 on 2022/5/3.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_LABEL_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_LABEL_H_
#include "element.h"
namespace gfx::compiler {
class label : public element {
public:
	label(element* parent, const std::string& name);

	std::string name() const;
private:
	std::string name_;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_LABEL_H_
