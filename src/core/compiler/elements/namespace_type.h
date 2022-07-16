//
// Created by 12132 on 2022/5/22.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_NAMESPACE_TYPE_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_NAMESPACE_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class namespace_type : public type {
public:
	explicit namespace_type(element* parent);

protected:
	bool on_initialize(result& r) override;
private:
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_NAMESPACE_TYPE_H_
