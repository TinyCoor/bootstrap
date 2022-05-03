//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_DIRECTIVE_H_
#define COMPILER_ELEMENTS_DIRECTIVE_H_
#include "block.h"
namespace gfx::compiler {

class directive : public block {
public:
	directive(block* parent, const std::string& name);

	~directive() override;

	std::string name() const;

private:
	std::string name_;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_DIRECTIVE_H_
