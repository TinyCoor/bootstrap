//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_ARRAY_TYPE_H_
#define COMPILER_ELEMENTS_ARRAY_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class array_type : public type {
public:
	array_type(element* parent, const std::string& name, compiler::type* entry_type);

	uint64_t size() const;

	void size(uint64_t value);

	compiler::type* entry_type();

private:
	uint64_t size_ = 0;
	compiler::type* entry_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ARRAY_TYPE_H_
